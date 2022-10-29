#include "FusionPCH.hpp"
#include "World.hpp"

namespace Fusion {

	World::World(const std::string& InName)
		: m_Name(InName)
	{
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
		FUSION_CORE_VERIFY(m_ActorIDMap.find(InActorID) != m_ActorIDMap.end());

		m_Registry.destroy(m_ActorIDMap.at(InActorID));
		m_Actors.erase(InActorID);
		m_ActorIDMap.find(InActorID);
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

}
