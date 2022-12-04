#include "FusionPCH.hpp"
#include "VulkanRenderTexture.hpp"

#include <vulkan/vulkan.h>
#include <VMA/vk_mem_alloc.h>

namespace Fusion {

	VulkanRenderTexture::VulkanRenderTexture(const RenderTextureInfo& InCreateInfo)
	    : m_CreateInfo(InCreateInfo)
	{
		/*uint32_t AttachmentCount = m_CreateInfo.ColorAttachments.size() + (IsDepthFormat(m_CreateInfo.DepthAttachment.Format) ? 1 : 0);

		VkFramebufferCreateInfo FrameBufferInfo = {};
		FrameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		FrameBufferInfo.renderPass = m_RenderPass;
		FrameBufferInfo.width = m_CreateInfo.Width;
		FrameBufferInfo.height = m_CreateInfo.Height;
		FrameBufferInfo.layers = 1;
		FrameBufferInfo.attachmentCount = AttachmentCount;
		FrameBufferInfo.pAttachments = &m_ImageViews[Idx];*/
	}

	void VulkanRenderTexture::Bind(CommandList* InCommandList)
	{
	}

	void VulkanRenderTexture::Unbind(CommandList* InCommandList)
	{
	}

	void VulkanRenderTexture::Clear()
	{
	}

	void VulkanRenderTexture::Resize(uint32_t InAttachmentIndex, uint32_t InFrameIndex, const ImageSize& InSize)
	{
	}

	void VulkanRenderTexture::TransitionImages(CommandList* InCommandList, EImageState InColorAttachmentState, EImageState InDepthStencilState)
	{
	}
}