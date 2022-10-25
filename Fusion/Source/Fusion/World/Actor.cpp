#include "FusionPCH.hpp"
#include "Actor.hpp"

namespace Fusion {
		
	Actor::Actor(World* InWorld, ActorID InActorID)
		: m_ActorID(InActorID), m_World(InWorld)
	{
	}

}
