#include "FusionPCH.hpp"
#include "Asset.hpp"
#include <random>

namespace Fusion {

	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_RandomEngine(s_RandomDevice());
	static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

	AssetHandle::AssetHandle(uint64_t InHandle)
		: m_Value(InHandle)
	{}

	AssetHandle::AssetHandle(EAssetType InType, uint64_t InID)
		: m_Value(0)
	{
		SetValue(InType, InID);
	}

	AssetHandle::AssetHandle(const AssetHandle& InOther)
		: m_Value(InOther.m_Value)
	{}

	AssetHandle::AssetHandle(EAssetType InType)
	{
		uint64_t ID = s_UniformDistribution(s_RandomEngine);
		SetValue(InType, ID);
	}

	void AssetHandle::SetValue(EAssetType InType, uint64_t InID)
	{
		m_Value = static_cast<uint64_t>(static_cast<uint16_t>(InType)) << TypeMask | (InID & IDMask);
	}

}
