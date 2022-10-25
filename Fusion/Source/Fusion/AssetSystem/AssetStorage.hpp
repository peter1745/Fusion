#pragma once

#include "Fusion/Core/Core.hpp"
#include "Asset.hpp"

namespace Fusion {

	class AssetStorage
	{
	public:
		template<typename T>
		requires std::derived_from<T, Asset>
		void AddAsset(const AssetContainer<T>& InAsset)
		{
			if (m_Storage.find(InAsset->Handle) != m_Storage.end())
			{
				return;
			}

			InAsset->SetRefCountChangedCallback(FUSION_BIND_FUNC(AssetStorage::OnAssetRefCountChanged));
			m_Storage[InAsset->Handle] = InAsset;
		}

		template<typename T>
		requires std::derived_from<T, Asset>
		AssetContainer<T> GetAsset(AssetHandle InHandle) const
		{
			FUSION_CORE_VERIFY(m_Storage.find(InHandle) != m_Storage.end(), "Asset not in storage!");
			return m_Storage.at(InHandle);
		}

	private:
		void OnAssetRefCountChanged(uint32_t InRefCount, const SharedAsset* InAsset, EAssetRefCountEventType InEventType);

	private:
		std::unordered_map<AssetHandle, AssetContainer<Asset>> m_Storage;
	};

}
