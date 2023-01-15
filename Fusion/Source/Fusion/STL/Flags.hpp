#pragma once

#include <cstdint>

#include <ostream>
#include <utility>

namespace Fusion {
	template <class T = std::uint32_t, T D = T { 0 }>
	struct Flags
	{
	public:
		using ValueT = T;

	public:
		constexpr Flags()
		    : m_Value(D) {}
		constexpr Flags(const ValueT& value)
		    : m_Value(value) {}
		constexpr Flags(ValueT&& value)
		    : m_Value(std::move(value)) {}
		template <class U, T E>
		constexpr Flags(const Flags<U, E>& flags)
		    : m_Value(flags.m_Value)
		{
		}
		template <class U, T E>
		constexpr Flags(Flags<U, E>&& flags)
		    : m_Value(std::move(flags.m_Value))
		{
		}

		constexpr Flags& operator=(const ValueT& value)
		{
			m_Value = value;
			return *this;
		}
		constexpr Flags& operator=(ValueT&& value)
		{
			m_Value = std::move(value);
			return *this;
		}
		template <class U, T E>
		constexpr Flags& operator=(const Flags<U, E>& flags)
		{
			m_Value = flags.m_Value;
			return *this;
		}
		template <class U, T E>
		constexpr Flags& operator=(Flags<U, E>&& flags)
		{
			m_Value = std::move(flags.m_Value);
			return *this;
		}

		constexpr operator ValueT&() { return m_Value; }
		constexpr ValueT& getValue() { return m_Value; }

		friend constexpr Flags operator~(const Flags& flags) { return ~flags.m_Value; }
		template <class U, T E>
		friend constexpr bool operator==(const Flags& lhs, const Flags<U, E>& rhs)
		{
			return lhs.m_Value == rhs.m_Value;
		}
		template <class U, T E>
		friend constexpr bool operator!=(const Flags& lhs, const Flags<U, E>& rhs)
		{
			return lhs.m_Value != rhs.m_Value;
		}
		template <class U, T E>
		friend constexpr bool operator<(const Flags& lhs, const Flags<U, E>& rhs)
		{
			return lhs.m_Value < rhs.m_Value;
		}
		template <class U, T E>
		friend constexpr bool operator<=(const Flags& lhs, const Flags<U, E>& rhs)
		{
			return lhs.m_Value <= rhs.m_Value;
		}
		template <class U, T E>
		friend constexpr bool operator>(const Flags& lhs, const Flags<U, E>& rhs)
		{
			return lhs.m_Value > rhs.m_Value;
		}
		template <class U, T E>
		friend constexpr bool operator>=(const Flags& lhs, const Flags<U, E>& rhs)
		{
			return lhs.m_Value >= rhs.m_Value;
		}
		template <class U, T E>
		friend constexpr Flags operator&(const Flags& lhs, const Flags<U, E>& rhs)
		{
			return lhs.m_Value & rhs.m_Value;
		}
		template <class U, T E>
		constexpr Flags& operator&=(const Flags<U, E>& rhs)
		{
			m_Value &= rhs.m_Value;
			return *this;
		}
		template <class U, T E>
		friend constexpr Flags operator|(const Flags& lhs, const Flags<U, E>& rhs)
		{
			return lhs.m_Value | rhs.m_Value;
		}
		template <class U, T E>
		constexpr Flags& operator|=(const Flags<U, E>& rhs)
		{
			m_Value |= rhs.m_Value;
			return *this;
		}
		template <class U, T E>
		friend constexpr Flags operator^(const Flags& lhs, const Flags<U, E>& rhs)
		{
			return lhs.m_Value ^ rhs.m_Value;
		}
		template <class U, T E>
		constexpr Flags& operator^=(const Flags<U, E>& rhs)
		{
			m_Value ^= rhs.m_Value;
			return *this;
		}
		friend constexpr Flags operator<<(const Flags& lhs, std::size_t count)
		{
			return lhs.m_Value << count;
		}
		constexpr Flags& operator<<=(std::size_t count)
		{
			m_Value <<= count;
			return *this;
		}
		friend constexpr Flags operator>>(const Flags& lhs, std::size_t count)
		{
			return lhs.m_Value >> count;
		}
		constexpr Flags& operator>>=(std::size_t count)
		{
			m_Value >>= count;
			return *this;
		}
		template <class U, T E>
		friend constexpr Flags operator-(const Flags& lhs, const Flags<U, E>& rhs)
		{
			return lhs.m_Value - rhs.m_Value;
		}
		template <class U, T E>
		constexpr Flags& operator-=(const Flags<U, E>& rhs)
		{
			m_Value -= rhs.m_Value;
			return *this;
		}
		template <class U, T E>
		friend constexpr Flags operator+(const Flags& lhs, const Flags<U, E>& rhs)
		{
			return lhs.m_Value + rhs.m_Value;
		}
		template <class U, T E>
		constexpr Flags& operator+=(const Flags<U, E>& rhs)
		{
			m_Value += rhs.m_Value;
			return *this;
		}
		template <class U, T E>
		friend constexpr Flags operator*(const Flags& lhs, const Flags<U, E>& rhs)
		{
			return lhs.m_Value * rhs.m_Value;
		}
		template <class U, T E>
		constexpr Flags& operator*=(const Flags<U, E>& rhs)
		{
			m_Value *= rhs.m_Value;
			return *this;
		}
		template <class U, T E>
		friend constexpr Flags operator/(const Flags& lhs, const Flags<U, E>& rhs)
		{
			return lhs.m_Value / rhs.m_Value;
		}
		template <class U, T E>
		constexpr Flags& operator/=(const Flags<U, E>& rhs)
		{
			m_Value /= rhs.m_Value;
			return *this;
		}

		friend std::ostream& operator<<(std::ostream& stream, const Flags& flags) { return stream << flags.m_Value; }

	public:
		ValueT m_Value;
	};
}