#pragma once

#include "VulkanDevice.h"
#include <vulkan/vulkan.h>

namespace Fusion {

	/*struct VulkanFramebufferSpecification
	{
		uint32_t Width;
		uint32_t Height;
		VkRenderPass RenderPass;
		VkImageView ImageView;
		VkFormat ImageFormat;
	};

	class VulkanFramebuffer
	{
	public:
		VulkanFramebuffer(const VulkanFramebufferSpecification& InSpecification, const Shared<VulkanDevice>& InDevice);
		~VulkanFramebuffer();

	private:
		VulkanFramebufferSpecification m_Specification;
		Shared<VulkanDevice> m_Device = nullptr;

		VkFramebuffer m_Framebuffer = VK_NULL_HANDLE;
	};*/

}
