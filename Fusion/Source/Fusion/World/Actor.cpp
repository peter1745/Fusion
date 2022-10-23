#include "FusionPCH.h"
#include "Actor.h"

namespace Fusion {
		
	Actor::Actor(World* InWorld, ActorID InActorID)
		: m_ActorID(InActorID), m_World(InWorld)
	{
	}

}
