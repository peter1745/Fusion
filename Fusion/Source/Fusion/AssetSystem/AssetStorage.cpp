#include "FusionPCH.hpp"
#include "AssetStorage.hpp"

namespace Fusion {

	void AssetStorage::OnAssetRefCountChanged(uint32_t InRefCount, const SharedAsset* InAsset, EAssetRefCountEventType InEventType)
	{
		if (InRefCount == 0)
			return;

		const Asset* AssetPtr = static_cast<const Asset*>(InAsset);

		if (m_Storage.find(AssetPtr->Handle) == m_Storage.end())
		{
			LogWarn("Fusion", "Notified about an asset ({}) that isn't managed by this storage instance!", AssetPtr->Handle);
			return;
		}

		switch (InEventType)
		{
		case EAssetRefCountEventType::Increased:
		{
			auto It = std::find_if(m_DestroyQueue.begin(), m_DestroyQueue.end(), [Handle = AssetPtr->Handle](const DestroyInfo& InDestroyInfo)
			{
				return InDestroyInfo.Handle == Handle;
			});

			if (It == m_DestroyQueue.end())
				break;

			m_DestroyQueue.erase(It);
			break;
		}
		case EAssetRefCountEventType::Decreased:
		{
			if (InRefCount != 1)
				return;

			m_DestroyQueue.push_back({ AssetPtr->Handle, m_CurrentFrame + 2U });
			break;
		}
		}
	}

	void AssetStorage::ProcessDestructionQueue()
	{
		for (auto It = m_DestroyQueue.begin(); It != m_DestroyQueue.end(); )
		{
			if (It->FrameIndex == m_CurrentFrame)
			{
				m_Storage.erase(It->Handle);
				It = m_DestroyQueue.erase(It);
			}
			else
			{
				It++;
			}
		}

		m_CurrentFrame++;
	}

}
