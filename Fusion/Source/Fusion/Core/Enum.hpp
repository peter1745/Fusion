#pragma once

#include <concepts>

namespace Fusion {

	template<typename TEnum>
	struct Enum
	{
		using UnderlyingType = std::underlying_type_t<TEnum>;

		constexpr Enum()
			: Value(0) {}

		constexpr Enum(UnderlyingType InValue)
			: Value(1 << InValue) {}

		constexpr Enum(TEnum InValue)
			: Value(1 << static_cast<UnderlyingType>(InValue)) {}

		template<typename TOtherEnum>
		friend constexpr Enum operator|(const Enum& InLHS, const Enum<TOtherEnum>& InRHS)
		{
			return InLHS.Value | InRHS.Value;
		}

		template<typename TOtherEnum>
		constexpr Enum& operator|=(const Enum<TOtherEnum>& InOther)
		{
			Value |= InOther.Value;
			return *this;
		}

		template<typename TOtherEnum>
		friend constexpr Enum operator|(const Enum& InLHS, TOtherEnum InRHS)
		{
			return InLHS.Value | 1 << static_cast<UnderlyingType>(InRHS);
		}

		template<typename TOtherEnum>
		constexpr Enum& operator|=(TOtherEnum InOther)
		{
			Value |= 1 << static_cast<UnderlyingType>(InOther);
			return *this;
		}

		template<typename TOtherEnum>
		friend constexpr Enum operator&(const Enum& InLHS, const Enum<TOtherEnum>& InRHS)
		{
			return InLHS.Value & InRHS.Value;
		}

		template<typename TOtherEnum>
		constexpr Enum& operator&=(const Enum<TOtherEnum>& InOther)
		{
			Value &= InOther.Value;
			return *this;
		}

		template<typename TOtherEnum>
		friend constexpr Enum operator&(const Enum& InLHS, TOtherEnum InRHS)
		{
			return InLHS.Value & 1 << static_cast<UnderlyingType>(InRHS);
		}

		template<typename TOtherEnum>
		constexpr Enum& operator&=(TOtherEnum InOther)
		{
			Value &= 1 << static_cast<UnderlyingType>(InOther);
			return *this;
		}
		
		template<typename TOtherEnum>
		constexpr bool operator!=(const Enum<TOtherEnum>& InOther)
		{
			return Value != InOther.Value;
		}

		template<typename TOtherEnum>
		constexpr bool operator!=(TOtherEnum InOther)
		{
			return Value != static_cast<UnderlyingType>(InOther);
		}

		template<typename TOtherEnum>
		constexpr bool operator==(const Enum<TOtherEnum>& InOther)
		{
			return Value == InOther.Value;
		}

		template<typename TOtherEnum>
		constexpr bool operator==(TOtherEnum InOther)
		{
			return Value == static_cast<UnderlyingType>(InOther);
		}

		UnderlyingType Value;
	};

}
