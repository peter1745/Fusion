#include "FusionPCH.hpp"
#include "UUID.hpp"

#include <random>

namespace Fusion {

	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_RandomEngine(s_RandomDevice());
	static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

	UUID::UUID()
		: m_ID(s_UniformDistribution(s_RandomEngine))
	{
	}

	UUID::UUID(uint64_t InID)
		: m_ID(InID)
	{
	}

	UUID::UUID(const UUID& InOther)
		: m_ID(InOther.m_ID)
	{
	}

}
