#pragma once

#include <vulkan/vulkan.h>

inline PFN_vkDestroySurfaceKHR evkDestroySurfaceKHR;
inline PFN_vkSetDebugUtilsObjectNameEXT evkSetDebugUtilsObjectNameEXT;
inline PFN_vkCmdPipelineBarrier2KHR evkCmdPipelineBarrier2KHR;
inline PFN_vkCmdBeginRenderingKHR evkCmdBeginRenderingKHR;
inline PFN_vkCmdEndRenderingKHR evkCmdEndRenderingKHR;

namespace Fusion {

	class VulkanLoader
	{
	public:
		static void LoadExtensionFunctions(VkDevice InDevice, VkInstance InInstance);
	};

}
