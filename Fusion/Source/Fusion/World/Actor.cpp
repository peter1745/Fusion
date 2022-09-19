#include "FusionPCH.h"
#include "Actor.h"

#include <random>

namespace Fusion {

	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_RandomEngine(s_RandomDevice());
	static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

	ActorID::ActorID()
		: m_ActorID(s_UniformDistribution(s_RandomEngine))
	{
	}

	ActorID::ActorID(uint64_t InActorID)
		: m_ActorID(InActorID)
	{
	}

	ActorID::ActorID(const ActorID& InOther)
		: m_ActorID(InOther.m_ActorID)
	{
	}

	Actor::Actor(World* InWorld, ActorID InActorID)
		: m_ActorID(InActorID), m_World(InWorld)
	{
	}

}
