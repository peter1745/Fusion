#pragma once

namespace Fusion {

	template<uint32_t Alignment>
	constexpr uint32_t Align(uint32_t InValue)
	{
		return (InValue + (Alignment - 1)) / Alignment * Alignment;
	}

}
