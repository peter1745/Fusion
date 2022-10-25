#include "FusionPCH.hpp"
#include "AssetStorage.hpp"

namespace Fusion {

	void AssetStorage::OnAssetRefCountChanged(uint32_t InRefCount, const SharedAsset* InAsset, EAssetRefCountEventType InEventType)
	{
		if (InEventType != EAssetRefCountEventType::Decreased || InRefCount != 1)
			return;

		const Asset* AssetPtr = static_cast<const Asset*>(InAsset);

		if (m_Storage.find(AssetPtr->Handle) == m_Storage.end())
		{
			FUSION_CORE_WARN("Notified about an asset ({}) that isn't managed by this storage instance!", AssetPtr->Handle);
			return;
		}

		m_Storage.erase(AssetPtr->Handle);
	}

}
