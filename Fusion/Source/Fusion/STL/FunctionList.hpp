#pragma once

#include <memory>
#include <functional>
#include <vector>

namespace Fusion {

	// NOTE(Peter): This *technically* doesn't have to be a wrapper, and this wrapper is most likely VERY inefficient
	//				It could be worth implementing this in a different way in the future

	template<typename TFunction>
	class FunctionList
	{
	public:
		using FuncType = std::function<TFunction>;
		using StorageType = std::vector<FuncType>;

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

		void AddFunction(const FuncType& InFunc)
		{
			m_Storage.push_back(InFunc);
		}

		void RemoveFunction(const FuncType& InFunc)
		{
			auto It = std::remove_if(m_Storage.begin(), m_Storage.end(), [&InFunc](const FuncType& OtherFunc)
			{
				return OtherFunc.target() == InFunc.target();
			});

			if (It != m_Storage.end())
				m_Storage.erase(It);
		}

		template<typename... TArgs>
		void Invoke(TArgs&&... InArgs)
		{
			for (const auto& Func : m_Storage)
				Func(std::forward<TArgs>(InArgs)...);
		}

	private:
		StorageType m_Storage;
	};

}
