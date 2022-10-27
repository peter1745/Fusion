#pragma once

#include "Fusion/Core/Core.hpp"
#include "Asset.hpp"
#include "AssetDatabank.hpp"
#include "AssetLoader.hpp"

namespace Fusion {

	class AssetStorage : public SharedObject
	{
	public:
		void AddDatabank(AssetHandle InHandle, const AssetDatabank& InDatabank)
		{
			if (m_Storage.find(InHandle) != m_Storage.end())
			{
				return;
			}

			m_Databanks[InHandle] = InDatabank;
		}

		const AssetDatabank& GetDatabank(AssetHandle InHandle) const
		{
			return m_Databanks.at(InHandle);
		}

		template<AssetType T>
		void AddAsset(AssetContainer<T> InAsset)
		{
			if (m_Storage.find(InAsset->Handle) != m_Storage.end())
			{
				return;
			}

			InAsset->SetRefCountChangedCallback(FUSION_BIND_FUNC(AssetStorage::OnAssetRefCountChanged));
			m_Storage[InAsset->Handle] = InAsset;
		}

		template<AssetType T>
		AssetContainer<T> GetAsset(AssetHandle InHandle)
		{
			if (m_Storage.find(InHandle) == m_Storage.end())
			{
				if (m_Databanks.find(InHandle) == m_Databanks.end())
				{
					FUSION_CORE_ERROR("Failed to find asset bank using handle {}", InHandle);
					return nullptr;
				}

				const auto& Databank = m_Databanks.at(InHandle);
				AssetContainer<T> NewAsset = AssetLoader::LoadFromFile<T>(Databank.FilePath);

				if (!NewAsset)
				{
					FUSION_CORE_ERROR("Failed to load asset {}", Databank.FilePath.string());
					return nullptr;
				}

				NewAsset->SetRefCountChangedCallback(FUSION_BIND_FUNC(AssetStorage::OnAssetRefCountChanged));
				m_Storage[InHandle] = NewAsset;
				return NewAsset;
			}

			return m_Storage.at(InHandle);
		}

		const std::unordered_map<AssetHandle, AssetDatabank>& GetDatabanks() const { return m_Databanks; }

	private:
		void OnAssetRefCountChanged(uint32_t InRefCount, const SharedAsset* InAsset, EAssetRefCountEventType InEventType);

		void ProcessDestructionQueue();

	private:
		std::unordered_map<AssetHandle, AssetDatabank> m_Databanks;
		std::unordered_map<AssetHandle, AssetContainer<Asset>> m_Storage;

		struct DestroyInfo
		{
			AssetHandle Handle;
			uint32_t FrameIndex;
		};
		std::vector<DestroyInfo> m_DestroyQueue;
		uint32_t m_CurrentFrame = 0;

		friend class Application;
	};

}
