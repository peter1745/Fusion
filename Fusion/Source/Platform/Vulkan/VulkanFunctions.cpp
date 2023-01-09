#include "FusionPCH.hpp"
#include "VulkanFunctions.hpp"

namespace Fusion {

	void VulkanLoader::LoadExtensionFunctions(VkDevice InDevice, VkInstance InInstance)
	{
		evkDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)(vkGetInstanceProcAddr(InInstance, "vkDestroySurfaceKHR"));
		FUSION_CORE_VERIFY(evkDestroySurfaceKHR != nullptr);

		evkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT) (vkGetDeviceProcAddr(InDevice, "vkSetDebugUtilsObjectNameEXT"));
		FUSION_CORE_VERIFY(evkSetDebugUtilsObjectNameEXT != nullptr);

		evkCmdPipelineBarrier2KHR = (PFN_vkCmdPipelineBarrier2KHR)(vkGetDeviceProcAddr(InDevice, "vkCmdPipelineBarrier2KHR"));
		FUSION_CORE_VERIFY(evkCmdPipelineBarrier2KHR != nullptr);

		evkCmdBeginRenderingKHR = (PFN_vkCmdBeginRenderingKHR) (vkGetDeviceProcAddr(InDevice, "vkCmdBeginRenderingKHR"));
		FUSION_CORE_VERIFY(evkCmdBeginRenderingKHR != nullptr);

		evkCmdEndRenderingKHR = (PFN_vkCmdEndRenderingKHR) (vkGetDeviceProcAddr(InDevice, "vkCmdEndRenderingKHR"));
		FUSION_CORE_VERIFY(evkCmdEndRenderingKHR != nullptr);
	}
}
