#pragma once

#include <stdint.h>

namespace Fission {

	class BodyID
	{
	public:
		static constexpr uint32_t InvalidID = 0xFFFF'FFFF;
		static constexpr uint32_t IndexMask = 0x7FFFFF;

	public:
		BodyID()
			: m_ID(InvalidID) {}

		BodyID(uint32_t InID)
			: m_ID(InID) {}

		BodyID(uint32_t InIndex, uint8_t InReserved)
			: m_ID((uint32_t(InReserved) << 24) | InIndex) {}

		uint32_t GetIndex() const { return m_ID & IndexMask; }
		uint32_t GetReserved() const { return m_ID >> 24; }

		bool IsInvalid() const { return m_ID == InvalidID; }

		inline bool operator==(const BodyID& InOther) const
		{
			return m_ID == InOther.m_ID;
		}

		inline bool operator!=(const BodyID& InOther) const
		{
			return m_ID != InOther.m_ID;
		}

		inline bool operator<(const BodyID& InOther) const
		{
			return m_ID < InOther.m_ID;
		}

		inline bool operator>(const BodyID& InOther) const
		{
			return m_ID < InOther.m_ID;
		}

	private:
		uint32_t m_ID;
	};

}
