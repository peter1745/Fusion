#pragma once

#include <concepts>
#include <guiddef.h>

namespace Fusion {

	template<typename T>
	concept ComInterface = requires(T* t)
	{
		{ t->AddRef() } -> std::same_as<DWORD>;
		{ t->Release() } -> std::same_as<DWORD>;
	};

	template<typename T, typename U>
	concept ComQueryableFrom = requires(U* u, T** t) { { u->QueryInterface(t) } -> std::same_as<HRESULT>; };

	template<typename T, typename U>
	concept ComCastableTo = ComInterface<U> &&
							ComInterface<T> &&
							(std::derived_from<U, T> || std::derived_from<T, U>) &&
							ComQueryableFrom<T, U>;

	template<ComInterface T>
	class D3D12ComPtr
	{
	public:
		static constexpr GUID UUID = __uuidof(T);

	public:
		D3D12ComPtr() = default;

		D3D12ComPtr(const D3D12ComPtr& InOther)
			: m_Ptr(InOther.m_Ptr)
		{
			AddRef();
		}

		D3D12ComPtr(D3D12ComPtr&& InOther) noexcept
			: m_Ptr(InOther.m_Ptr)
		{
			InOther.m_Ptr = nullptr;
		}

		template<ComCastableTo<T> U>
		D3D12ComPtr(D3D12ComPtr<U>& InOther)
		{
			if (InOther.IsValid())
				InOther->QueryInterface(&m_Ptr);
		}

		D3D12ComPtr& operator=(const D3D12ComPtr& InOther)
		{
			Release();
			m_Ptr = InOther.m_Ptr;
			AddRef();
			return *this;
		}

		D3D12ComPtr& operator=(D3D12ComPtr&& InOther) noexcept
		{
			Release();
			m_Ptr = InOther.m_Ptr;
			InOther.m_Ptr = nullptr;
			return *this;
		}

		template<ComCastableTo<T> U>
		D3D12ComPtr& operator=(D3D12ComPtr<U>& InOther)
		{
			Release();
			if (InOther.IsValid())
				InOther->QueryInterface(&m_Ptr);
			return *this;
		}

		~D3D12ComPtr()
		{
			Release();
		}

		void AddRef()
		{
			if (m_Ptr)
				m_Ptr->AddRef();
		}

		void Release()
		{
			if (m_Ptr)
				m_Ptr->Release();

			m_Ptr = nullptr;
		}

		bool IsValid() const { return m_Ptr != nullptr; }

		T* Get() { return m_Ptr; }
		const T* Get() const { return m_Ptr; }

		T* operator->() { return m_Ptr; }
		const T* operator->() const { return m_Ptr; }
		T& operator*() { return *m_Ptr; }
		const T& operator*() const { return *m_Ptr; }
		operator T& () { return *m_Ptr; }
		operator const T&() const { return *m_Ptr; }

		operator T* () { return m_Ptr; }
		operator const T* () const { return m_Ptr; }

		operator T** () { return &m_Ptr; }
		operator const T* const*() const { return &m_Ptr; }
		operator void** () { return reinterpret_cast<void**>(&m_Ptr); }
		operator const void* const* () const { return reinterpret_cast<const void* const*>(&m_Ptr); }

		operator const GUID& () const { return UUID; }

	private:
		T* m_Ptr = nullptr;
	};

}
