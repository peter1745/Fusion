#pragma once

#include "Fission/Body/BodyAllocator.hpp"
#include "Fission/Body/Body.hpp"
#include "Fission/Collision/SphereShape.hpp"
#include "Fission/Collision/BroadPhase/BroadPhase.hpp"

#include <unordered_map>

namespace Fission {

	struct WorldSettings
	{
		glm::vec3 Gravity = { 0.0f, -9.81f, 0.0f };
	};

	struct BodyContact
	{
		glm::vec3 PointA = { 0.0f, 0.0f, 0.0f };
		glm::vec3 PointB = { 0.0f, 0.0f, 0.0f };
		glm::vec3 LocalPointA = { 0.0f, 0.0f, 0.0f };
		glm::vec3 LocalPointB = { 0.0f, 0.0f, 0.0f };

		glm::vec3 Normal = { 0.0f, 0.0f, 0.0f };

		float Distance = 0.0f;

		float CombinedRestitution = 0.0f;
		float CombinedFriction = 0.0f;

		float TimeOfImpact = 0.0f;

		BodyID BodyA = BodyID::InvalidID;
		BodyID BodyB = BodyID::InvalidID;
	};

	class DynamicsWorld
	{
	public:
		void SetSettings(const WorldSettings& InSettings) { m_Settings = InSettings; }

		void Initialize(uint32_t InMaxBodies = 100, EBroadPhaseAlgorithm InBroadPhase = EBroadPhaseAlgorithm::SAP);
		void Shutdown();

		BodyID CreateBody(const BodySettings& InSettings);
		void RemoveBody(const BodyID& InBodyID);
		Body* GetBody(const BodyID& InBodyID);

		void Simulate(float InDeltaTime);

		void AddImpulse(const BodyID& InBodyID, const glm::vec3& InImpulse, const glm::vec3& InPoint);
		void AddLinearImpulse(const BodyID& InBodyID, const glm::vec3& InImpulse);
		void AddAngularImpulse(const BodyID& InBodyID, const glm::vec3& InImpulse);

		glm::vec3 GetBodyCenterOfMass(const BodyID& InBodyID) const;
		glm::vec3 GetLocalBodyCenterOfMass(const BodyID& InBodyID) const;

		glm::vec3 ConvertWorldSpaceToBodySpace(const BodyID& InBodyID, const glm::vec3& InWorldPoint) const;
		glm::vec3 ConvertBodySpaceToWorldSpace(const BodyID& InBodyID, const glm::vec3& InWorldPoint) const;

		glm::mat3 GetInverseInertiaTensor(const BodyID& InBodyID) const;
		glm::mat3 GetBodyInverseInertiaTensor(const BodyID& InBodyID) const;

	private:
		bool Intersects(Body* InBody0, Body* InBody1, float InDeltaTime, BodyContact& OutContact) const;

		bool RaySphere(const glm::vec3& InRayStart, const glm::vec3& InRayDir, const glm::vec3& InSphereCenter, float InSphereRadius, float& OutT1, float& OutT2) const;
		bool SphereSphereDynamic(const SphereShape* InSphere0, const SphereShape* InSphere1, const glm::vec3& InLocation0, const glm::vec3& InLocation1, const glm::vec3& InVelocity0, const glm::vec3& InVelocity1, float InDeltaTime, BodyContact& InContact) const;

		void GenerateContact(BodyContact& InContact, const Body& InBody0, const Body& InBody1) const;
		void ResolveContact(const BodyContact& InContact);

		void IntegrateVelocities(Body* InBody, float InDeltaTime) const;

		struct SupportPoint
		{
			glm::vec3 MinkowskiPoint;
			glm::vec3 PointA;
			glm::vec3 PointB;

			bool operator==(const SupportPoint& InOther) const
			{
				return PointA == InOther.PointA && PointB == InOther.PointB && MinkowskiPoint == InOther.MinkowskiPoint;
			}
		};
		SupportPoint GetSupportPoint(const Body* InBody0, const Body* InBody1, const glm::vec3& InDirection, float InBias) const;

	private:
		WorldSettings m_Settings;

		BodyAllocator* m_BodyAllocator = nullptr;
		BroadPhase* m_BroadPhase = nullptr;

		std::vector<BodyID> m_ActiveBodies;
	};

}
