#pragma once

#include <functional>

namespace Fusion {

	enum class EAssetRefCountEventType { Increased, Decreased };
	
	class SharedAsset;
	using AssetRefCountCallback = std::function<void(uint32_t, const SharedAsset*, EAssetRefCountEventType)>;

	class SharedAsset
	{
	private:
		void IncreaseReferenceCount()
		{
			++m_RefCount;

			if (m_RefCountCallback)
				m_RefCountCallback(m_RefCount.load(), this, EAssetRefCountEventType::Increased);
		}

		void DecreaseReferenceCount()
		{
			--m_RefCount;

			if (m_RefCountCallback)
				m_RefCountCallback(m_RefCount.load(), this, EAssetRefCountEventType::Decreased);
		}

		uint32_t GetReferenceCount() const { return m_RefCount.load(); }

		void SetRefCountChangedCallback(const AssetRefCountCallback& InCallback) { m_RefCountCallback = InCallback; }

		mutable std::atomic<uint32_t> m_RefCount = 0;

		AssetRefCountCallback m_RefCountCallback;

		template<typename T, typename TBaseType>
		friend class Shared;

		friend class AssetStorage;
	};

}
