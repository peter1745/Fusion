#include "FusionPCH.h"
#include "World.h"

namespace Fusion {

	Shared<Actor> World::CreateActor(const std::string& InName)
	{
		Shared<Actor> NewActor(new Actor(this));
		NewActor->Name = InName;
		
		m_Actors[NewActor->m_ActorID] = NewActor;
		m_ActorIDMap[NewActor->m_ActorID] = m_Registry.create();

		AddActorComponent<ActorComponent>(NewActor->m_ActorID)->ID = NewActor->m_ActorID;
		AddActorComponent<TransformComponent>(NewActor->m_ActorID);

		return NewActor;
	}

}
