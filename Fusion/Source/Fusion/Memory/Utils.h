#pragma once

namespace Fusion {

	template<uint32_t Alignment>
	constexpr uint32_t Align(uint32_t InValue)
	{
		return (InValue + (Alignment - 1)) / Alignment * Alignment;
	}

	constexpr uint32_t Align(uint32_t InValue, uint32_t InAlignment)
	{
		return (InValue + (InAlignment - 1)) / InAlignment * InAlignment;

	}

}
