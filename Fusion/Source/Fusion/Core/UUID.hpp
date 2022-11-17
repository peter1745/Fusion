#pragma once

#include <xhash>

namespace Fusion {

	class UUID
	{
	public:
		static const uint64_t Invalid = 0;

	public:
		UUID();
		UUID(uint64_t InID);
		UUID(const UUID& InOther);

		operator uint64_t() { return m_ID; }
		operator const uint64_t() const { return m_ID; }
	private:
		uint64_t m_ID;
	};

}

namespace std {

	template<>
	struct hash<Fusion::UUID>
	{
		size_t operator()(const Fusion::UUID& InUUID) const noexcept
		{
			return static_cast<size_t>(static_cast<uint64_t>(InUUID));
		}
	};

}
