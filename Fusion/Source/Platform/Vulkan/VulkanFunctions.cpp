#include "FusionPCH.hpp"
#include "VulkanFunctions.hpp"

namespace Fusion {

	void VulkanLoader::LoadExtensionFunctions(VkDevice InDevice)
	{
		fpSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT) (vkGetDeviceProcAddr(InDevice, "vkSetDebugUtilsObjectNameEXT"));
		FUSION_CORE_VERIFY(fpSetDebugUtilsObjectNameEXT != nullptr);

		fpCmdPipelineBarrier2KHR = (PFN_vkCmdPipelineBarrier2KHR) (vkGetDeviceProcAddr(InDevice, "vkCmdPipelineBarrier2KHR"));
		FUSION_CORE_VERIFY(fpCmdPipelineBarrier2KHR != nullptr);
	}
}
