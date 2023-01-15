#include "FusionPCH.hpp"
#include "World.hpp"
#include "Components/AllComponents.hpp"

#include <Fission/Body/Body.hpp>
#include <Fission/Collision/SphereShape.hpp>
#include <Fission/Collision/BoxShape.hpp>

namespace Fusion {

	template<typename... TComponents>
	static void CopyComponent(entt::registry& InDestination, entt::registry& InSource, const std::unordered_map<ActorID, entt::entity>& InActorMap)
	{
		([&]()
		{
			auto CompView = InSource.view<TComponents>();
			for (const auto SourceEnttID : CompView)
			{
				const auto& SourceComp = CompView.template get<TComponents>(SourceEnttID);
				auto DestinationEnttID = InActorMap.at(InSource.get<ActorComponent>(SourceEnttID).ID);
				InDestination.emplace_or_replace<TComponents>(DestinationEnttID, SourceComp);
			}
		}(), ...);
	}

	template<typename... TComponents>
	static void CopyComponent(entt::registry& InDestination, entt::registry& InSource, const std::unordered_map<ActorID, entt::entity>& InActorMap, [[maybe_unused]] ComponentGroup<TComponents...> InCompGroup)
	{
		CopyComponent<TComponents...>(InDestination, InSource, InActorMap);
	}

	static void CopyComponents(entt::registry& InDestination, entt::registry& InSource, const std::unordered_map<ActorID, entt::entity>& InActorMap)
	{
		CopyComponent(InDestination, InSource, InActorMap, AllComponents{});
	}

	World::World(const std::string& InName)
		: m_Name(InName)
	{
	}

	World::~World()
	{
	}

	void World::StartSimulation()
	{
		m_State = WorldStates::Simulating;

		m_PhysicsWorld.Initialize(5000);

		auto PhysicsActors = FindAllActorsWith<PhysicsBodyComponent>();
		for (auto Actor : PhysicsActors)
		{
			const auto* ActorTransform = FindActorComponent<TransformComponent>(Actor->GetActorID());
			const auto* PhysicsBodyComp = FindActorComponent<PhysicsBodyComponent>(Actor->GetActorID());

			Fission::BodySettings Settings = {};
			Settings.InitialLocation = ActorTransform->Location;
			Settings.InitialOrientation = ActorTransform->GetRotation();
			Settings.Mass = PhysicsBodyComp->Mass;
			
			const auto* BoxShape = FindActorComponent<BoxShapeComponent>(Actor->GetActorID());
			const auto* SphereShape = FindActorComponent<SphereShapeComponent>(Actor->GetActorID());
			if (BoxShape)
			{
				Settings.Shape = new Fission::BoxShape(BoxShape->HalfSize);
				Settings.Shape->SetRestitution(1.0f);
				Settings.Shape->SetFriction(0.1f);
			}
			else if (SphereShape)
			{
				float LargestComponent = glm::max(ActorTransform->Scale.x, glm::max(ActorTransform->Scale.y, ActorTransform->Scale.z));
				Settings.Shape = new Fission::SphereShape(LargestComponent * SphereShape->Radius);
				Settings.Shape->SetRestitution(1.0f);
				Settings.Shape->SetFriction(0.1f);
			}

			m_ActorIDToPhysicsBodyIDMap[Actor->GetActorID()] = m_PhysicsWorld.CreateBody(Settings);
		}
	}

	void World::Simulate(float InDeltaTime)
	{
		if (!(m_State & WorldStates::Simulating))
			return;

		m_PhysicsWorld.Simulate(InDeltaTime);

		// Update Actor Locations
		for (const auto& [ActorID, BodyID] : m_ActorIDToPhysicsBodyIDMap)
		{
			auto* ActorTransform = FindActorComponent<TransformComponent>(ActorID);
			Fission::Body* Body = m_PhysicsWorld.GetBody(BodyID);
			ActorTransform->Location = Body->Location;
		}
	}

	void World::StopSimulation()
	{
		for (const auto& [ActorID, BodyID] : m_ActorIDToPhysicsBodyIDMap)
			m_PhysicsWorld.RemoveBody(BodyID);
		m_ActorIDToPhysicsBodyIDMap.clear();
		m_PhysicsWorld.Shutdown();
	}

	Shared<Actor> World::CreateActorWithID(ActorID InActorID, const std::string& InName, const Shared<Actor>& InParent /*= nullptr*/)
	{
		Shared<Actor> NewActor(new Actor(this, InActorID));
		NewActor->Name = InName;

		m_Actors[NewActor->m_ActorID] = NewActor;
		m_ActorIDMap[NewActor->m_ActorID] = m_Registry.create();

		AddActorComponent<ActorComponent>(NewActor->m_ActorID)->ID = NewActor->m_ActorID;
		AddActorComponent<TransformComponent>(NewActor->m_ActorID);
		RelationshipComponent* RelationshipComp = AddActorComponent<RelationshipComponent>(NewActor->m_ActorID);

		if (InParent)
		{
			RelationshipComponent* ParentRelationshipComp = FindActorComponent<RelationshipComponent>(InParent->m_ActorID);

			RelationshipComp->ParentID = InParent->m_ActorID;

			if (ParentRelationshipComp->FirstChildID != ActorID::Invalid)
			{
				ActorID CurrentChildID = ParentRelationshipComp->FirstChildID;
				while (CurrentChildID != ActorID::Invalid)
				{
					RelationshipComponent* CurrentChildComp = FindActorComponent<RelationshipComponent>(CurrentChildID);

					if (CurrentChildComp->NextSiblingID == ActorID::Invalid)
					{
						// Found last child
						CurrentChildComp->NextSiblingID = NewActor->m_ActorID;
						RelationshipComp->PrevSiblingID = CurrentChildID;
						break;
					}

					CurrentChildID = CurrentChildComp->NextSiblingID;
				}
			}
			else
			{
				ParentRelationshipComp->FirstChildID = NewActor->m_ActorID;
			}
		}

		return NewActor;
	}

	Shared<Actor> World::FindActorWithID(ActorID InActorID) const
	{
		if (auto Iter = m_Actors.find(InActorID); Iter != m_Actors.end())
			return Iter->second;

		return nullptr;
	}

	void World::DestroyActor(ActorID InActorID)
	{
		CoreVerify(m_ActorIDMap.find(InActorID) != m_ActorIDMap.end());

		m_Registry.destroy(m_ActorIDMap.at(InActorID));
		m_Actors.erase(InActorID);
		m_ActorIDMap.find(InActorID);
	}

	void World::Restore(Shared<World> InOther)
	{
		// Clear the existing world
		m_ActorIDMap.clear();
		m_Actors.clear();
		m_Registry.clear();

		// Copy InOther
		m_Registry.reserve(InOther->m_Registry.size());

		std::unordered_map<ActorID, entt::entity> EnttIDMap;
		EnttIDMap.reserve(InOther->m_Registry.size());
		auto View = InOther->m_Registry.view<ActorComponent>();
		for (auto EnttID : View)
		{
			ActorID ID = View.get<ActorComponent>(EnttID).ID;
			const auto& Actor = InOther->m_Actors.at(ID);
			SharedActor NewActor = CreateActorWithID(ID, Actor->Name);
			EnttIDMap[ID] = m_ActorIDMap.at(ID);
		}

		CopyComponents(m_Registry, InOther->m_Registry, EnttIDMap);

		m_State = InOther->m_State;
	}

	Shared<Actor> World::GetMainCameraActor() const
	{
		auto EnttView = m_Registry.view<ActorComponent, CameraComponent>();

		for (auto EnttID : EnttView)
		{
			const auto& CameraComp = m_Registry.get<CameraComponent>(EnttID);

			if (!CameraComp.IsMainCamera || !CameraComp.IsActive)
				continue;

			const auto& ActorComp = m_Registry.get<ActorComponent>(EnttID);
			return m_Actors.at(ActorComp.ID);
		}

		return nullptr;
	}

	void World::Clear()
	{
		m_ActorIDMap.clear();
		m_Actors.clear();
		m_Registry.clear();
	}

	Shared<World> World::Copy(Shared<World> InOther)
	{
		Shared<World> NewWorld = Shared<World>::Create(InOther->m_Name);
		NewWorld->Restore(InOther);
		return NewWorld;
	}

}
