#pragma once

#include <atomic>

namespace Fusion {

	class SharedStorage
	{
	public:
		void IncreaseReferenceCount() { ++m_RefCount; }
		void DecreaseReferenceCount() { --m_RefCount; }
		uint32_t GetReferenceCount() const { return m_RefCount.load(); }

	private:
		mutable std::atomic<uint32_t> m_RefCount = 0;
	};

	template<typename T>
	class Shared
	{
	public:
		Shared()
			: m_Ptr(nullptr), m_Storage(nullptr)
		{
		}

		Shared(std::nullptr_t)
			: m_Ptr(nullptr), m_Storage(nullptr)
		{
		}

		Shared(T* InPtr)
			: m_Ptr(InPtr)
		{
			m_Storage = new SharedStorage();
			m_Storage->IncreaseReferenceCount();
		}

		Shared(const Shared<T>& InOther)
			: m_Ptr(InOther.m_Ptr), m_Storage(InOther.m_Storage)
		{
			IncreaseReferenceCount();
		}

		Shared(Shared<T>&& InOther) noexcept
		{
			m_Ptr = InOther.m_Ptr;
			m_Storage = InOther.m_Storage;
			InOther.m_Ptr = nullptr;
			InOther.m_Storage = nullptr;
		}

		template<typename TOther>
		Shared(const Shared<TOther>& InOther)
		{
			m_Ptr = static_cast<T*>(InOther.m_Ptr);
			m_Storage = InOther.m_Storage;
			IncreaseReferenceCount();
		}

		template<typename TOther>
		Shared(Shared<TOther>&& InOther) noexcept
		{
			m_Ptr = static_cast<T*>(InOther.m_Ptr);
			m_Storage = InOther.m_Storage;
			InOther.m_Ptr = nullptr;
			InOther.m_Storage = nullptr;
		}

		~Shared()
		{
			DecreaseReferenceCount();
		}

		Shared& operator=(std::nullptr_t)
		{
			DecreaseReferenceCount();
			m_Storage = nullptr;
			m_Ptr = nullptr;
			return *this;
		}

		Shared& operator=(const Shared<T>& InOther)
		{
			InOther.IncreaseReferenceCount();
			DecreaseReferenceCount();

			m_Ptr = InOther.m_Ptr;
			m_Storage = InOther.m_Storage;
			return *this;
		}

		Shared& operator=(Shared<T>&& InOther) noexcept
		{
			DecreaseReferenceCount();

			m_Ptr = InOther.m_Ptr;
			m_Storage = InOther.m_Storage;
			InOther.m_Ptr = nullptr;
			InOther.m_Storage = nullptr;

			return *this;
		}

		template<typename TOther>
		Shared& operator=(const Shared<TOther>& InOther)
		{
			InOther.IncreaseReferenceCount();
			DecreaseReferenceCount();

			m_Ptr = static_cast<T*>(InOther.m_Ptr);
			m_Storage = InOther.m_Storage;

			return *this;
		}

		template<typename TOther>
		Shared& operator=(Shared<TOther>&& InOther) noexcept
		{
			DecreaseReferenceCount();
			
			m_Ptr = static_cast<T*>(InOther.m_Ptr);
			m_Storage = InOther.m_Storage;
			InOther.m_Ptr = nullptr;
			InOther.m_Storage = nullptr;
			
			return *this;
		}

		operator bool() { return m_Ptr != nullptr && m_Storage != nullptr; }
		operator bool() const { return m_Ptr != nullptr && m_Storage != nullptr; }

		T* operator->() { return m_Ptr; }
		const T* operator->() const { return m_Ptr; }

		T& operator*() { return *m_Ptr; }
		const T& operator*() const { return *m_Ptr; }

		T* Get() { return m_Ptr; }
		const T* Get() const { return m_Ptr; }

		void Reset(T* InPtr = nullptr)
		{
			DecreaseReferenceCount();
			m_Ptr = InPtr;
			m_Storage = new SharedStorage();
			m_Storage->IncreaseReferenceCount();
		}

		template<typename TOther>
		Shared<TOther> As() const { return Shared<TOther>(*this); }

	public:
		template<typename... TArgs>
		static Shared<T> Create(TArgs&&... InArgs)
		{
			return Shared<T>(new T(std::forward<TArgs>(InArgs)...));
		}

	private:
		void IncreaseReferenceCount()
		{
			if (m_Storage)
				m_Storage->IncreaseReferenceCount();
		}

		void DecreaseReferenceCount()
		{
			if (!m_Storage)
				return;

			m_Storage->DecreaseReferenceCount();

			// Free the object and the storage if reference count is 0
			if (m_Storage->GetReferenceCount() == 0)
			{
				delete m_Ptr;
				m_Ptr = nullptr;

				delete m_Storage;
				m_Storage = nullptr;
			}
		}

	private:
		template<class TOther>
		friend class Shared;

		mutable T* m_Ptr;
		SharedStorage* m_Storage;
	};

}
