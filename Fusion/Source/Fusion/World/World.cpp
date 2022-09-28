#include "FusionPCH.h"
#include "World.h"

namespace Fusion {

	World::World()
	{
	}

	Shared<Actor> World::CreateActor(const std::string& InName, const Shared<Actor>& InParent)
	{
		Shared<Actor> NewActor(new Actor(this));
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

}
