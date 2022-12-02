#pragma once

#include <stdint.h>
#include <vulkan/vulkan.h>

namespace Fusion {

	struct QueueFamilyInfo
	{
		uint32_t QueueIndex = UINT32_MAX;
		bool SupportsPresent = false;
		bool SupportsGraphics = false;
		bool SupportCompute = false;

		VkQueue Queue = VK_NULL_HANDLE;

		bool IsValid() const { return QueueIndex != UINT32_MAX; }
	};

}
