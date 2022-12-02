#pragma once

#include <vulkan/vulkan.h>

inline PFN_vkSetDebugUtilsObjectNameEXT fpSetDebugUtilsObjectNameEXT;
inline PFN_vkCmdPipelineBarrier2KHR fpCmdPipelineBarrier2KHR;

namespace Fusion {

	class VulkanLoader
	{
	public:
		static void LoadExtensionFunctions(VkDevice InDevice);
	};

}
