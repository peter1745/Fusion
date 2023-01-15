#include "FusionPCH.hpp"
#include "VulkanFunctions.hpp"
#include "Fusion/Core/Assert.hpp"

namespace Fusion {

	void VulkanLoader::LoadExtensionFunctions(VkDevice InDevice, VkInstance InInstance)
	{
		evkDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)(vkGetInstanceProcAddr(InInstance, "vkDestroySurfaceKHR"));
		CoreVerify(evkDestroySurfaceKHR != nullptr);

		evkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT) (vkGetDeviceProcAddr(InDevice, "vkSetDebugUtilsObjectNameEXT"));
		CoreVerify(evkSetDebugUtilsObjectNameEXT != nullptr);

		evkCmdPipelineBarrier2KHR = (PFN_vkCmdPipelineBarrier2KHR)(vkGetDeviceProcAddr(InDevice, "vkCmdPipelineBarrier2KHR"));
		CoreVerify(evkCmdPipelineBarrier2KHR != nullptr);

		evkCmdBeginRenderingKHR = (PFN_vkCmdBeginRenderingKHR) (vkGetDeviceProcAddr(InDevice, "vkCmdBeginRenderingKHR"));
		CoreVerify(evkCmdBeginRenderingKHR != nullptr);

		evkCmdEndRenderingKHR = (PFN_vkCmdEndRenderingKHR) (vkGetDeviceProcAddr(InDevice, "vkCmdEndRenderingKHR"));
		CoreVerify(evkCmdEndRenderingKHR != nullptr);
	}
}
