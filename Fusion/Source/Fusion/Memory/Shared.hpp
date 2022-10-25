#pragma once

#include <atomic>
#include <concepts>

namespace Fusion {

	class SharedObject
	{
	private:
		void IncreaseReferenceCount() { ++m_RefCount; }
		void DecreaseReferenceCount() { --m_RefCount; }
		uint32_t GetReferenceCount() const { return m_RefCount.load(); }

		mutable std::atomic<uint32_t> m_RefCount = 0;

		template<typename T>
		friend class Shared;
	};

	template<typename T>
	class Shared
	{
	public:
		Shared()
			: m_Ptr(nullptr)
		{
		}

		Shared(std::nullptr_t)
			: m_Ptr(nullptr)
		{
		}

		Shared(T* InPtr)
			: m_Ptr(InPtr)
		{
			IncreaseReferenceCount();
		}

		Shared(const Shared<T>& InOther)
			: m_Ptr(InOther.m_Ptr)
		{
			IncreaseReferenceCount();
		}

		Shared(Shared<T>&& InOther) noexcept
		{
			m_Ptr = InOther.m_Ptr;
			InOther.m_Ptr = nullptr;
		}

		template<typename TOther>
		Shared(const Shared<TOther>& InOther)
		{
			m_Ptr = static_cast<T*>(InOther.m_Ptr);
			IncreaseReferenceCount();
		}

		template<typename TOther>
		Shared(Shared<TOther>&& InOther) noexcept
		{
			m_Ptr = static_cast<T*>(InOther.m_Ptr);
			InOther.m_Ptr = nullptr;
		}

		~Shared()
		{
			DecreaseReferenceCount();
		}

		Shared& operator=(std::nullptr_t)
		{
			DecreaseReferenceCount();
			m_Ptr = nullptr;
			return *this;
		}

		Shared& operator=(const Shared<T>& InOther)
		{
			DecreaseReferenceCount();
			m_Ptr = InOther.m_Ptr;
			IncreaseReferenceCount();
			return *this;
		}

		Shared& operator=(Shared<T>&& InOther) noexcept
		{
			DecreaseReferenceCount();
			m_Ptr = InOther.m_Ptr;
			InOther.m_Ptr = nullptr;
			return *this;
		}

		template<typename TOther>
		Shared& operator=(const Shared<TOther>& InOther)
		{
			DecreaseReferenceCount();
			m_Ptr = static_cast<T*>(InOther.m_Ptr);
			IncreaseReferenceCount();
			return *this;
		}

		template<typename TOther>
		Shared& operator=(Shared<TOther>&& InOther) noexcept
		{
			DecreaseReferenceCount();
			m_Ptr = static_cast<T*>(InOther.m_Ptr);
			InOther.m_Ptr = nullptr;
			return *this;
		}

		operator bool() { return m_Ptr != nullptr; }
		operator bool() const { return m_Ptr != nullptr; }

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
			IncreaseReferenceCount();
		}

		template<typename TOther>
		Shared<TOther> As() const { return Shared<TOther>(*this); }

		bool operator==(const Shared<T>& InOther) const { return m_Ptr == InOther.m_Ptr; }
		bool operator!=(const Shared<T>& InOther) const { return !(*this == InOther); }

	public:
		template<typename... TArgs>
			requires std::constructible_from<T, TArgs...>
		static Shared<T> Create(TArgs&&... InArgs)
		{
			return Shared<T>(new T(std::forward<TArgs>(InArgs)...));
		}

	private:
		void IncreaseReferenceCount()
		{
			if (m_Ptr)
				m_Ptr->IncreaseReferenceCount();
		}

		void DecreaseReferenceCount()
		{
			if (!m_Ptr)
				return;

			m_Ptr->DecreaseReferenceCount();

			// Free the object and the storage if reference count is 0
			if (m_Ptr->GetReferenceCount() == 0)
			{
				delete m_Ptr;
				m_Ptr = nullptr;
			}
		}

	private:
		template<class TOther>
		friend class Shared;

		mutable T* m_Ptr;
	};

}
