#include "FusionPCH.h"
#include "VulkanRenderPass.h"
#include "VulkanContext.h"

namespace Fusion {

	VulkanRenderPass::VulkanRenderPass(const RenderPassSpecification& InSpecification)
		: m_Specification(InSpecification)
	{
		auto Swapchain = VulkanContext::Get().GetSwapchain();
		m_Specification.TargetImageSize = m_Specification.IsSwapchainTarget ? Swapchain->GetSwapchainExtent() : m_Specification.TargetImageSize;
	}

	void VulkanRenderPass::Begin(VkCommandBuffer InCommandBuffer)
	{
		auto Swapchain = VulkanContext::Get().GetSwapchain();
		VkImageView TargetImageView = m_Specification.IsSwapchainTarget ? Swapchain->GetCurrentImageView() : m_Specification.TargetImageView;

		VkRenderingAttachmentInfo ColorAttachmentInfo = {};
		ColorAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		ColorAttachmentInfo.clearValue = { {{ 0.0f, 0.0f, 0.0f, 1.0f }} };
		ColorAttachmentInfo.imageView = TargetImageView;
		ColorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		ColorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		ColorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		VkRenderingInfo RenderingInfo = {};
		RenderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		RenderingInfo.flags = 0;
		RenderingInfo.renderArea.offset = { 0, 0 };
		RenderingInfo.renderArea.extent = m_Specification.TargetImageSize;
		RenderingInfo.layerCount = 1;
		RenderingInfo.viewMask = 0;
		RenderingInfo.colorAttachmentCount = 1;
		RenderingInfo.pColorAttachments = &ColorAttachmentInfo;
		RenderingInfo.pDepthAttachment = nullptr;
		RenderingInfo.pStencilAttachment = nullptr;

		vkCmdBeginRendering(InCommandBuffer, &RenderingInfo);
	}

	void VulkanRenderPass::End(VkCommandBuffer InCommandBuffer)
	{
		vkCmdEndRendering(InCommandBuffer);
	}

	VkImageView VulkanRenderPass::GetImageView() const
	{
		auto Swapchain = VulkanContext::Get().GetSwapchain();
		return m_Specification.IsSwapchainTarget ? Swapchain->GetCurrentImageView() : m_Specification.TargetImageView;
	}

}
