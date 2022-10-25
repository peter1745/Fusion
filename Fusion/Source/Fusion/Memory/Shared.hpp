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

		template<typename T, typename TBaseType>
		friend class Shared;
	};

	template<typename T, typename TBaseType = SharedObject>
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
			: m_Ptr(static_cast<TBaseType*>(InPtr))
		{
			IncreaseReferenceCount();
		}

		Shared(const Shared<T, TBaseType>& InOther)
			: m_Ptr(static_cast<TBaseType*>(InOther.m_Ptr))
		{
			IncreaseReferenceCount();
		}

		Shared(Shared<T, TBaseType>&& InOther) noexcept
		{
			m_Ptr = InOther.m_Ptr;
			InOther.m_Ptr = nullptr;
		}

		template<typename TOther, typename TOtherBase>
		Shared(const Shared<TOther, TOtherBase>& InOther)
		{
			m_Ptr = static_cast<TBaseType*>(InOther.m_Ptr);
			IncreaseReferenceCount();
		}

		template<typename TOther, typename TOtherBase>
		Shared(Shared<TOther, TOtherBase>&& InOther) noexcept
		{
			m_Ptr = static_cast<TBaseType*>(InOther.m_Ptr);
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

		Shared& operator=(const Shared<T, TBaseType>& InOther)
		{
			DecreaseReferenceCount();
			m_Ptr = InOther.m_Ptr;
			IncreaseReferenceCount();
			return *this;
		}

		Shared& operator=(Shared<T, TBaseType>&& InOther) noexcept
		{
			DecreaseReferenceCount();
			m_Ptr = InOther.m_Ptr;
			InOther.m_Ptr = nullptr;
			return *this;
		}

		template<typename TOther, typename TOtherBase>
		Shared& operator=(const Shared<TOther, TOtherBase>& InOther)
		{
			DecreaseReferenceCount();
			m_Ptr = static_cast<TBaseType*>(InOther.m_Ptr);
			IncreaseReferenceCount();
			return *this;
		}

		template<typename TOther, typename TOtherBase>
		Shared& operator=(Shared<TOther, TOtherBase>&& InOther) noexcept
		{
			DecreaseReferenceCount();
			m_Ptr = static_cast<T*>(InOther.m_Ptr);
			InOther.m_Ptr = nullptr;
			return *this;
		}

		operator bool() { return m_Ptr != nullptr; }
		operator bool() const { return m_Ptr != nullptr; }

		T* operator->() { return static_cast<T*>(m_Ptr); }
		const T* operator->() const { return static_cast<const T*>(m_Ptr); }

		T& operator*() { return *static_cast<T*>(m_Ptr); }
		const T& operator*() const { return *static_cast<const T*>(m_Ptr); }

		T* Get() { return static_cast<T*>(m_Ptr); }
		const T* Get() const { return static_cast<T*>(m_Ptr); }

		void Reset(T* InPtr = nullptr)
		{
			DecreaseReferenceCount();
			m_Ptr = InPtr;
			IncreaseReferenceCount();
		}

		template<typename TOther>
		Shared<TOther, TBaseType> As() const { return Shared<TOther, TBaseType>(*this); }

		bool operator==(const Shared<T>& InOther) const { return m_Ptr == InOther.m_Ptr; }
		bool operator!=(const Shared<T>& InOther) const { return !(*this == InOther); }

	public:
		template<typename... TArgs>
		static Shared<T> Create(TArgs&&... InArgs) requires std::constructible_from<T, TArgs...>
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
		template<class TOther, class TBaseType>
		friend class Shared;

		mutable TBaseType* m_Ptr;
	};

}
