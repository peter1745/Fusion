#include "DynamicsWorld.hpp"
#include "Fission/Collision/BroadPhase/SweepAndPruneBroadPhase.hpp"

#include <iostream>

#include <tracy/Tracy.hpp>

namespace Fission {

	void DynamicsWorld::Initialize(uint32_t InMaxBodies, EBroadPhaseAlgorithm InBroadPhase)
	{
		m_BodyAllocator = new BodyAllocator(InMaxBodies);
		m_ActiveBodies.reserve(InMaxBodies);

		switch (InBroadPhase)
		{
		case EBroadPhaseAlgorithm::SAP:
			m_BroadPhase = new SweepAndPruneBroadPhase();
			break;
		}

		m_BroadPhase->Initialize(m_BodyAllocator);
	}

	void DynamicsWorld::Shutdown()
	{
		for (const auto& ActiveActorID : m_ActiveBodies)
			m_BodyAllocator->RemoveBody(ActiveActorID);

		m_ActiveBodies.clear();

		delete m_BodyAllocator;
	}

	BodyID DynamicsWorld::CreateBody(const BodySettings& InSettings)
	{
		Body* NewBody = m_BodyAllocator->AllocateBody();

		if (NewBody == nullptr)
		{
			return BodyID::InvalidID;
		}

		NewBody->Location = InSettings.InitialLocation;
		NewBody->Orientation = InSettings.InitialOrientation;
		NewBody->InverseMass = InSettings.Mass != 0.0f ? 1.0f / InSettings.Mass : 0.0f;
		NewBody->Shape = InSettings.Shape;

		m_ActiveBodies.push_back(NewBody->ID);

		m_BroadPhase->AddBodies(&NewBody->ID, 1);

		return NewBody->ID;
	}

	void DynamicsWorld::RemoveBody(const BodyID& InBodyID)
	{
		if (InBodyID.IsInvalid())
		{
			//__debugbreak();
			return;
		}

		std::erase_if(m_ActiveBodies, [&InBodyID](const BodyID& InOther) { return InBodyID == InOther; });
		m_BodyAllocator->RemoveBody(InBodyID);
	}

	Body* DynamicsWorld::GetBody(const BodyID& InBodyID)
	{
		return m_BodyAllocator->GetBody(InBodyID);
	}

	int CompareContacts(const void* InP1, const void* InP2)
	{
		const BodyContact* Contact0 = static_cast<const BodyContact*>(InP1);
		const BodyContact* Contact1 = static_cast<const BodyContact*>(InP2);

		if (Contact0->TimeOfImpact < Contact1->TimeOfImpact)
			return -1;

		if (Contact0->TimeOfImpact == Contact1->TimeOfImpact)
			return 0;

		return 1;
	}

	void DynamicsWorld::Simulate(float InDeltaTime)
	{
		ZoneScoped;

		{
			ZoneScopedN("Apply Gravity");

			for (const auto& ActiveBodyID : m_ActiveBodies)
			{
				Body* ActiveBody = m_BodyAllocator->GetBody(ActiveBodyID);

				// NOTE(Peter): This would most likely not be necessary once static bodies are apart of the statics world
				if (ActiveBody->InverseMass == 0.0f)
					continue;

				// Don't compute gravity as a force or impulse (it already is one, although should take a look at PhysX and Jolt to see how they handle it)
				ActiveBody->LinearVelocity += m_Settings.Gravity * InDeltaTime;
			}
		}

		m_BroadPhase->Execute(InDeltaTime);

		const int32_t MaxContacts = m_ActiveBodies.size() * m_ActiveBodies.size();
		std::vector<BodyContact> Contacts;
		Contacts.reserve(MaxContacts);

		{
			ZoneScopedN("Narrow Phase");

			for (const auto& Pair : m_BroadPhase->GetCollisionPairs())
			{
				const BodyID& BodyID0 = m_ActiveBodies[Pair.IDIndex0];
				const BodyID& BodyID1 = m_ActiveBodies[Pair.IDIndex1];

				Body* Body0 = m_BodyAllocator->GetBody(BodyID0);
				Body* Body1 = m_BodyAllocator->GetBody(BodyID1);

				if (Body0->InverseMass == 0.0f && Body1->InverseMass == 0.0f)
					continue;

				BodyContact Contact = {};
				if (Intersects(Body0, Body1, InDeltaTime, Contact))
					Contacts.push_back(Contact);
			}

			if (Contacts.size() > 1)
				std::qsort(Contacts.data(), Contacts.size(), sizeof(BodyContact), CompareContacts);
		}

		float AccumulatedTime = 0.0f;
		{
			ZoneScopedN("Contact Resolution");

			for (const auto& Contact : Contacts)
			{
				float DeltaTime = Contact.TimeOfImpact - AccumulatedTime;

				Body* Body0 = m_BodyAllocator->GetBody(Contact.BodyA);
				Body* Body1 = m_BodyAllocator->GetBody(Contact.BodyB);

				if (Body0->InverseMass == 0.0f && Body1->InverseMass == 0.0f)
					continue;

				for (const auto& ActiveBodyID : m_ActiveBodies)
				{
					Body* ActiveBody = m_BodyAllocator->GetBody(ActiveBodyID);
					IntegrateVelocities(ActiveBody, DeltaTime);
				}

				ResolveContact(Contact);
				AccumulatedTime += DeltaTime;
			}
		}

		float TimeRemaining = InDeltaTime - AccumulatedTime;
		if (TimeRemaining > 0.0f)
		{
			for (const auto& ActiveBodyID : m_ActiveBodies)
			{
				Body* ActiveBody = m_BodyAllocator->GetBody(ActiveBodyID);
				IntegrateVelocities(ActiveBody, TimeRemaining);
			}
		}
	}

	void DynamicsWorld::AddImpulse(const BodyID& InBodyID, const glm::vec3& InImpulse, const glm::vec3& InPoint)
	{
		// Linear Impulse
		AddLinearImpulse(InBodyID, InImpulse);

		// Angular Impulse
		glm::vec3 AngularImpulse = glm::cross(InPoint - GetBodyCenterOfMass(InBodyID), InImpulse);
		AddAngularImpulse(InBodyID, AngularImpulse);
	}

	void DynamicsWorld::AddLinearImpulse(const BodyID& InBodyID, const glm::vec3& InImpulse)
	{
		Body* Body = m_BodyAllocator->GetBody(InBodyID);

		if (Body->InverseMass == 0.0f)
			return;

		Body->LinearVelocity += InImpulse * Body->InverseMass;
	}

	void DynamicsWorld::AddAngularImpulse(const BodyID& InBodyID, const glm::vec3& InImpulse)
	{
		Body* Body = m_BodyAllocator->GetBody(InBodyID);

		if (Body->InverseMass == 0.0f)
			return;

		Body->AngularVelocity += GetInverseInertiaTensor(InBodyID) * InImpulse;
		const float MaxAngularVelocity = 30.0f;
		if (glm::dot(Body->AngularVelocity, Body->AngularVelocity) > MaxAngularVelocity * MaxAngularVelocity)
		{
			Body->AngularVelocity = glm::normalize(Body->AngularVelocity);
			Body->AngularVelocity *= MaxAngularVelocity;
		}
	}

	glm::vec3 DynamicsWorld::GetBodyCenterOfMass(const BodyID& InBodyID) const
	{
		const Body* Body = m_BodyAllocator->GetBody(InBodyID);
		const glm::vec3& CenterOfMass = Body->Shape->GetCenterOfMass();
		return Body->Location + (Body->Orientation * CenterOfMass);
	}

	glm::vec3 DynamicsWorld::GetLocalBodyCenterOfMass(const BodyID& InBodyID) const
	{
		const Body* Body = m_BodyAllocator->GetBody(InBodyID);
		return Body->Shape->GetCenterOfMass();
	}

	glm::vec3 DynamicsWorld::ConvertWorldSpaceToBodySpace(const BodyID& InBodyID, const glm::vec3& InWorldPoint) const
	{
		const Body* Body = m_BodyAllocator->GetBody(InBodyID);
		return glm::inverse(Body->Orientation) * (InWorldPoint - GetBodyCenterOfMass(InBodyID));
	}

	glm::vec3 DynamicsWorld::ConvertBodySpaceToWorldSpace(const BodyID& InBodyID, const glm::vec3& InWorldPoint) const
	{
		const Body* Body = m_BodyAllocator->GetBody(InBodyID);
		return (Body->Orientation * InWorldPoint) + GetBodyCenterOfMass(InBodyID);
	}

	glm::mat3 DynamicsWorld::GetInverseInertiaTensor(const BodyID& InBodyID) const
	{
		const Body* Body = m_BodyAllocator->GetBody(InBodyID);
		glm::mat3 BodyInvInertiaTensor = GetBodyInverseInertiaTensor(InBodyID);
		glm::mat3 Orientation = glm::mat3_cast(Body->Orientation);
		return Orientation * BodyInvInertiaTensor * glm::transpose(Orientation);
	}

	glm::mat3 DynamicsWorld::GetBodyInverseInertiaTensor(const BodyID& InBodyID) const
	{
		const Body* Body = m_BodyAllocator->GetBody(InBodyID);
		const auto& ShapeInertiaTensor = Body->Shape->GetInertiaTensor();
		return glm::inverse(ShapeInertiaTensor) * Body->InverseMass;
	}

	bool DynamicsWorld::Intersects(Body* InBody0, Body* InBody1, float InDeltaTime, BodyContact& OutContact) const
	{
		OutContact.BodyA = InBody0->ID;
		OutContact.BodyB = InBody1->ID;

		if (InBody0->Shape->GetType() == EShapeType::Sphere && InBody1->Shape->GetType() == EShapeType::Sphere)
		{
			const SphereShape* SphereShape0 = static_cast<const SphereShape*>(InBody0->Shape);
			const SphereShape* SphereShape1 = static_cast<const SphereShape*>(InBody1->Shape);

			if (SphereSphereDynamic(SphereShape0, SphereShape1, InBody0->Location, InBody1->Location, InBody0->LinearVelocity, InBody1->LinearVelocity, InDeltaTime, OutContact))
			{
				// TODO(Peter): Possibly do sqrt(restitution0 * restitution1)
				OutContact.CombinedRestitution = SphereShape0->GetRestitution() * SphereShape1->GetRestitution();
				OutContact.CombinedFriction = SphereShape0->GetFriction() * SphereShape1->GetFriction();

				IntegrateVelocities(InBody0, OutContact.TimeOfImpact);
				IntegrateVelocities(InBody1, OutContact.TimeOfImpact);

				OutContact.LocalPointA = ConvertWorldSpaceToBodySpace(InBody0->ID, OutContact.PointA);
				OutContact.LocalPointB = ConvertWorldSpaceToBodySpace(InBody1->ID, OutContact.PointB);

				OutContact.Normal = glm::normalize(InBody0->Location - InBody1->Location);

				IntegrateVelocities(InBody0, -OutContact.TimeOfImpact);
				IntegrateVelocities(InBody1, -OutContact.TimeOfImpact);

				glm::vec3 AB = InBody1->Location - InBody0->Location;
				OutContact.Distance = glm::length(AB) - (SphereShape0->GetRadius() + SphereShape1->GetRadius());
				return true;
			}
		}

		return false;
	}

	bool DynamicsWorld::RaySphere(const glm::vec3& InRayStart, const glm::vec3& InRayDir, const glm::vec3& InSphereCenter, float InSphereRadius, float& OutT1, float& OutT2) const
	{
		glm::vec3 Distance = InSphereCenter - InRayStart;
		float A = glm::dot(InRayDir, InRayDir);
		float B = glm::dot(Distance, InRayDir);
		float C = glm::dot(Distance, Distance) - InSphereRadius * InSphereRadius;
		float Delta = B * B - A * C;

		if (Delta < 0.0f)
			return false;

		float InverseA = 1.0f / A;
		float DeltaSqrt = glm::sqrt(Delta);
		OutT1 = InverseA * (B - DeltaSqrt);
		OutT2 = InverseA * (B + DeltaSqrt);
		return true;
	}

	bool DynamicsWorld::SphereSphereDynamic(const SphereShape* InSphere0, const SphereShape* InSphere1, const glm::vec3& InLocation0, const glm::vec3& InLocation1, const glm::vec3& InVelocity0, const glm::vec3& InVelocity1, float InDeltaTime, BodyContact& InContact) const
	{
		glm::vec3 RelativeVelocity = InVelocity0 - InVelocity1;
		glm::vec3 StartPoint = InLocation0;
		glm::vec3 EndPoint = InLocation0 + RelativeVelocity * InDeltaTime;
		glm::vec3 RayDirection = EndPoint - StartPoint;

		float T0 = 0.0f;
		float T1 = 0.0f;

		if (glm::dot(RayDirection, RayDirection) < 0.001f * 0.001f)
		{
			// Ray is too short, check if already intersecting
			glm::vec3 AB = InLocation1 - InLocation0;
			float Radius = InSphere0->GetRadius() + InSphere1->GetRadius();

			if (glm::dot(AB, AB) > Radius * Radius)
				return false;
		}
		else if (!RaySphere(StartPoint, RayDirection, InLocation1, InSphere0->GetRadius() + InSphere1->GetRadius(), T0, T1))
		{
			return false;
		}

		// [0...1] -> [0...InDeltaTime]
		T0 *= InDeltaTime;
		T1 *= InDeltaTime;

		if (T1 < 0.0f)
			return false;

		InContact.TimeOfImpact = glm::max(0.0f, T0);

		if (InContact.TimeOfImpact > InDeltaTime)
			return false;

		glm::vec3 NewLocation0 = InLocation0 + InVelocity0 * InContact.TimeOfImpact;
		glm::vec3 NewLocation1 = InLocation1 + InVelocity1 * InContact.TimeOfImpact;
		glm::vec3 AB = glm::normalize(NewLocation1 - NewLocation0);

		InContact.PointA = NewLocation0 + AB * InSphere0->GetRadius();
		InContact.PointB = NewLocation1 - AB * InSphere1->GetRadius();
		return true;
	}

	void DynamicsWorld::GenerateContact(BodyContact& InContact, const Body& InBody0, const Body& InBody1) const
	{
		InContact.Normal = glm::normalize(InBody1.Location - InBody0.Location);

		auto* Shape0 = static_cast<SphereShape*>(InBody0.Shape);
		auto* Shape1 = static_cast<SphereShape*>(InBody1.Shape);

		float Radius0 = Shape0->GetRadius();
		float Radius1 = Shape1->GetRadius();

		InContact.PointA = InBody0.Location + InContact.Normal * Radius0;
		InContact.PointB = InBody1.Location - InContact.Normal * Radius1;

		// TODO(Peter): Possibly do sqrt(restitution0 * restitution1)
		InContact.CombinedRestitution = Shape0->GetRestitution() * Shape1->GetRestitution();
		InContact.CombinedFriction = Shape0->GetFriction() * Shape1->GetFriction();

		InContact.BodyA = InBody0.ID;
		InContact.BodyB = InBody1.ID;
	}

	void DynamicsWorld::ResolveContact(const BodyContact& InContact)
	{
		Body* Body0 = m_BodyAllocator->GetBody(InContact.BodyA);
		Body* Body1 = m_BodyAllocator->GetBody(InContact.BodyB);

		glm::mat3 InvInertia0 = GetInverseInertiaTensor(InContact.BodyA);
		glm::mat3 InvInertia1 = GetInverseInertiaTensor(InContact.BodyB);

		glm::vec3 RA = InContact.PointA - GetBodyCenterOfMass(InContact.BodyA);
		glm::vec3 RB = InContact.PointB - GetBodyCenterOfMass(InContact.BodyB);

		glm::vec3 AngularJ0 = glm::cross(InvInertia0 * glm::cross(RA, InContact.Normal), RA);
		glm::vec3 AngularJ1 = glm::cross(InvInertia1 * glm::cross(RB, InContact.Normal), RB);
		float AngularFactor = glm::dot(AngularJ0 + AngularJ1, InContact.Normal);

		// Get the world space velocity of the motion and rotation
		glm::vec3 Velocity0 = Body0->LinearVelocity + glm::cross(Body0->AngularVelocity, RA);
		glm::vec3 Velocity1 = Body1->LinearVelocity + glm::cross(Body1->AngularVelocity, RB);

		// Calculate collision impulse
		glm::vec3 VelocityDiff = Velocity0 - Velocity1;
		float ImpulseJ = (1.0f + InContact.CombinedRestitution) * glm::dot(VelocityDiff, InContact.Normal) / (Body0->InverseMass + Body1->InverseMass + AngularFactor);
		glm::vec3 Impulse = InContact.Normal * ImpulseJ;

		AddImpulse(InContact.BodyA, Impulse * -1.0f, InContact.PointA);
		AddImpulse(InContact.BodyB, Impulse * 1.0f, InContact.PointB);

		// Calculate friction impulse
		glm::vec3 VelocityNormal = InContact.Normal * glm::dot(InContact.Normal, VelocityDiff);
		glm::vec3 VelocityTangent = VelocityDiff - VelocityNormal;
		glm::vec3 RelativeVelocityTangent = VelocityTangent;
		if (glm::dot(RelativeVelocityTangent, RelativeVelocityTangent) != 0.0f)
			RelativeVelocityTangent = glm::normalize(RelativeVelocityTangent);

		glm::vec3 Inertia0 = glm::cross(InvInertia0 * glm::cross(RA, RelativeVelocityTangent), RA);
		glm::vec3 Inertia1 = glm::cross(InvInertia1 * glm::cross(RB, RelativeVelocityTangent), RB);
		float InvInertia = glm::dot(Inertia0 + Inertia1, RelativeVelocityTangent);

		float ReducedMass = 1.0f / (Body0->InverseMass + Body1->InverseMass + InvInertia);
		glm::vec3 ImpulseFriction = VelocityTangent * ReducedMass * InContact.CombinedFriction;

		AddImpulse(InContact.BodyA, ImpulseFriction * -1.0f, InContact.PointA);
		AddImpulse(InContact.BodyB, ImpulseFriction * 1.0f, InContact.PointB);

		if (InContact.TimeOfImpact == 0.0f)
		{
			const float Contribution0 = Body0->InverseMass / (Body0->InverseMass + Body1->InverseMass);
			const float Contribution1 = Body1->InverseMass / (Body0->InverseMass + Body1->InverseMass);

			glm::vec3 Distance = InContact.PointB - InContact.PointA;
			Body0->Location += Distance * Contribution0;
			Body1->Location -= Distance * Contribution1;
		}
	}

	void DynamicsWorld::IntegrateVelocities(Body* InBody, float InDeltaTime) const
	{
		if (InBody->InverseMass == 0.0f)
			return;

		InBody->Location += InBody->LinearVelocity * InDeltaTime;

		glm::vec3 CenterOfMass = GetBodyCenterOfMass(InBody->ID);
		glm::vec3 CenterOfMassToLocation = InBody->Location - CenterOfMass;

		// Calculate new angular velocity
		glm::mat3 Orientation = glm::mat3_cast(InBody->Orientation);
		glm::mat3 InertiaTensor = Orientation * InBody->Shape->GetInertiaTensor() * glm::transpose(Orientation);
		glm::vec3 Alpha = glm::inverse(InertiaTensor) * glm::cross(InBody->AngularVelocity, InertiaTensor * InBody->AngularVelocity);
		InBody->AngularVelocity += Alpha * InDeltaTime;

		// Update orientation
		glm::vec3 DeltaAngle = InBody->AngularVelocity * InDeltaTime;
		glm::quat DeltaOrientation = glm::quat(glm::length(DeltaAngle), DeltaAngle);
		InBody->Orientation = glm::normalize(DeltaOrientation * InBody->Orientation);

		// Calculate new location
		InBody->Location = CenterOfMass + (DeltaOrientation * CenterOfMassToLocation);
		InBody->Location += m_Settings.Gravity * 0.5f * InDeltaTime * InDeltaTime;
	}

	DynamicsWorld::SupportPoint DynamicsWorld::GetSupportPoint(const Body* InBody0, const Body* InBody1, const glm::vec3& InDirection, float InBias) const
	{
		DynamicsWorld::SupportPoint Point = {};

		Point.PointA = InBody0->Shape->GetFurthestPoint(InBody0->Location, InDirection, InBody0->Orientation, InBias);
		Point.PointB = InBody1->Shape->GetFurthestPoint(InBody1->Location, InDirection * -1.0f, InBody1->Orientation, InBias);
		Point.MinkowskiPoint = Point.PointA - Point.PointB;

		return Point;
	}

}
