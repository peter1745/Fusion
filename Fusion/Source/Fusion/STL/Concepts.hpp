#pragma once

#include <concepts>

namespace Fusion {

	template<typename T>
	concept Hashable = requires(T t)
	{
		{ std::hash<T>{}(t) } -> std::convertible_to<size_t>;
	};

	template<typename T>
	concept Comparable = requires(T t, T b)
	{
		{ t == b } -> std::convertible_to<bool>;
		{ t != b } -> std::convertible_to<bool>;
	};

}
