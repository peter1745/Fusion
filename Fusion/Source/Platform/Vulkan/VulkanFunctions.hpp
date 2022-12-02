#pragma once

#include <vulkan/vulkan.h>

inline PFN_vkDestroySurfaceKHR evkDestroySurfaceKHR;
inline PFN_vkSetDebugUtilsObjectNameEXT evkSetDebugUtilsObjectNameEXT;
inline PFN_vkCmdPipelineBarrier2KHR evkCmdPipelineBarrier2KHR;

namespace Fusion {

	class VulkanLoader
	{
	public:
		static void LoadExtensionFunctions(VkDevice InDevice, VkInstance InInstance);
	};

}
