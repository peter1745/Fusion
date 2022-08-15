#pragma once

#include <vulkan/vulkan.h>

namespace Fusion {

	struct RenderPassSpecification
	{
		bool IsSwapchainTarget = false;
		VkImageView TargetImageView = VK_NULL_HANDLE;
		VkExtent2D TargetImageSize = { 0, 0 };
	};

	class VulkanRenderPass
	{
	public:
		VulkanRenderPass(const RenderPassSpecification& InSpecification);
		~VulkanRenderPass() = default;

		void Begin(VkCommandBuffer InCommandBuffer);
		void End(VkCommandBuffer InCommandBuffer);

		VkImageView GetImageView() const;

	private:
		RenderPassSpecification m_Specification;
	};

}
