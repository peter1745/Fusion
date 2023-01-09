#pragma once

#include <memory>
#include <functional>
#include <unordered_map>
#include <ranges>

#include "Fusion/Core/UUID.hpp"

namespace Fusion {

	// NOTE(Peter): This *technically* doesn't have to be a wrapper, and this wrapper is most likely VERY inefficient
	//				It could be worth implementing this in a different way in the future

	template<typename TFunction>
	class FunctionList
	{
	public:
		using KeyType = UUID;
		using FuncType = std::function<TFunction>;
		using StorageType = std::unordered_map<KeyType, FuncType>;

	public:
		FunctionList() = default;
		FunctionList(const FunctionList& InOther)
			: m_Storage(InOther.m_Storage) {}
		FunctionList(FunctionList&& InOther) noexcept
			: m_Storage(std::move(InOther.m_Storage)) {}

		FunctionList& operator=(const FunctionList& InOther)
		{
			m_Storage = StorageType(InOther.m_Storage);
			return *this;
		}

		FunctionList& operator=(FunctionList&& InOther) noexcept
		{
			m_Storage = std::move(InOther.m_Storage);
			return *this;
		}

		KeyType AddFunction(const FuncType& InFunc)
		{
			KeyType Key = KeyType();
			m_Storage[Key] = InFunc;
			return Key;
		}

		void RemoveFunction(const KeyType& InKey)
		{
			m_Storage.erase(InKey);
		}

		template<typename... TArgs>
		void Invoke(TArgs&&... InArgs)
		{
			for (const auto& Func : m_Storage | std::views::values)
				Func(std::forward<TArgs>(InArgs)...);
		}

	private:
		StorageType m_Storage;
	};

}
