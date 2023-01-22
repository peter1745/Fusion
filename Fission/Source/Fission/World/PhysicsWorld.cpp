#include "FissionPCH.hpp"
#include "PhysicsWorld.hpp"

#include "Fission/Body/DynamicBody.hpp"
#include "Fission/Integration/SemiImplicitEulerIntegrator.hpp"
#include "Fission/Collision/CollisionChecker.hpp"
#include "Fission/Collision/ContactResolver.hpp"

namespace Fission {

	void PhysicsWorld::Init()
	{
		// NOTE(Peter): Currently I only support Semi-Implicit Euler integration
		//				but in the future I'll implement Velocity Verlet and RK4
		//				so that I can compare them.
		m_BodyIntegrator = std::make_unique<SemiImplicitEulerIntegrator>();
	}

	void PhysicsWorld::Shutdown()
	{
		m_Bodies.clear();
	}

	void PhysicsWorld::Simulate(float InDeltaTime)
	{
		m_DeltaTime = InDeltaTime;

		for (auto& BodyPtr : m_Bodies)
		{
			if (BodyPtr->GetType() != EBodyType::Dynamic)
				continue;

			m_BodyIntegrator->IntegrateForceTorqueAndDrag(BodyPtr->As<DynamicBody>(), m_Settings.Gravity, InDeltaTime);
		}

		ContactResolver Resolver;
		for (size_t I0 = 0; I0 < m_Bodies.size(); I0++)
		{
			auto& Body0 = m_Bodies[I0];
			Shape* Shape0 = Body0->GetShape();

			// If this body doesn't have a shape it can't collide with anything
			if (Shape0 == nullptr)
				continue;

			for (size_t I1 = I0 + 1; I1 < m_Bodies.size(); I1++)
			{
				auto& Body1 = m_Bodies[I1];

				if (Body0->GetType() == EBodyType::Static && Body1->GetType() == EBodyType::Static)
					continue;

				Shape* Shape1 = Body1->GetShape();

				bool IsColliding = false;

				switch (Shape0->GetType())
				{
				case EShapeType::Sphere:
				{
					SphereShape* Sphere = static_cast<SphereShape*>(Shape0);
					
					switch (Shape1->GetType())
					{
					case EShapeType::Sphere:
					{
						SphereShape* Sphere1 = static_cast<SphereShape*>(Shape1);
						IsColliding = CollisionChecker::TestSphereSphereCollision(Body0->GetPosition(), Sphere, Body1->GetPosition(), Sphere1);
						break;
					}
					case EShapeType::Box:
					{
						BoxShape* Box = static_cast<BoxShape*>(Shape1);
						IsColliding = CollisionChecker::TestSphereBoxCollision(Body0->GetPosition(), Sphere, Body1->GetPosition(), Box);
						break;
					}
					}
					break;
				}
				case EShapeType::Box:
				{
					BoxShape* Box = static_cast<BoxShape*>(Shape0);

					switch (Shape1->GetType())
					{
					case EShapeType::Sphere:
					{
						SphereShape* Sphere = static_cast<SphereShape*>(Shape1);
						IsColliding = CollisionChecker::TestSphereBoxCollision(Body1->GetPosition(), Sphere, Body0->GetPosition(), Box);
						break;
					}

					case EShapeType::Box:
					{
						BoxShape* Box1 = static_cast<BoxShape*>(Shape1);
						IsColliding = CollisionChecker::TestBoxBoxCollision(Body0->GetPosition(), Box, Body1->GetPosition(), Box1);
						break;
					}
					}
				}
				}

				if (IsColliding)
				{
					Resolver.GenerateContact(Body0.get(), Body1.get());

					/*if (Body0->GetType() == EBodyType::Dynamic)
						Body0->As<DynamicBody>()->SetLinearVelocity(glm::zero<glm::vec3>());

					if (Body1->GetType() == EBodyType::Dynamic)
						Body1->As<DynamicBody>()->SetLinearVelocity(glm::zero<glm::vec3>());*/
				}
			}
		}

		Resolver.ResolveContacts();

		for (auto& BodyPtr : m_Bodies)
		{
			if (BodyPtr->GetType() != EBodyType::Dynamic)
				continue;

			m_BodyIntegrator->IntegrateVelocity(BodyPtr->As<DynamicBody>(), InDeltaTime);
		}
	}

	Body* PhysicsWorld::CreateBody(const BodySettings& InSettings)
	{
		switch (InSettings.BodyType)
		{
		case EBodyType::Static:
		{
			auto NewBody = std::make_unique<Body>();
			NewBody->m_Position = InSettings.Position;
			NewBody->m_Rotation = InSettings.Rotation;
			NewBody->m_Shape = InSettings.CollisionShape;
			return m_Bodies.emplace_back(std::move(NewBody)).get();
		}
		case EBodyType::Dynamic:
		{
			auto NewBody = std::make_unique<DynamicBody>();
			NewBody->m_Position = InSettings.Position;
			NewBody->m_Rotation = InSettings.Rotation;
			NewBody->m_Shape = InSettings.CollisionShape;
			NewBody->m_InvMass = 1.0f / InSettings.Mass;
			return m_Bodies.emplace_back(std::move(NewBody)).get();
		}
		}

		return nullptr;
	}

}
