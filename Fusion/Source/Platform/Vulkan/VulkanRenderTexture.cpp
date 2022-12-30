#include "FusionPCH.hpp"
#include "VulkanRenderTexture.hpp"
#include "VulkanDevice.hpp"

#include "Fusion/Renderer/Renderer.hpp"

#include <vulkan/vulkan.h>
#include <VMA/vk_mem_alloc.h>

namespace Fusion {

	VulkanRenderTexture::VulkanRenderTexture(const Shared<Device>& InDevice, const RenderTextureInfo& InCreateInfo)
	    : m_CreateInfo(InCreateInfo)
	{
		uint32_t AttachmentCount = m_CreateInfo.ColorAttachments.size() + (IsDepthFormat(m_CreateInfo.DepthAttachment.Format) ? 1 : 0);
		uint32_t FrameCount = Renderer::GetCurrent().GetFramesInFlight();

		auto Device = InDevice.As<VulkanDevice>();

		m_Images.resize(InCreateInfo.ColorAttachments.size() * FrameCount);
		m_ImageViews.resize(InCreateInfo.ColorAttachments.size() * FrameCount);
		
		std::vector<VkAttachmentDescription> AttachmentDescriptions(InCreateInfo.ColorAttachments.size());
		std::vector<VkAttachmentReference> AttachmentReferences(InCreateInfo.ColorAttachments.size());
		std::vector<VkSubpassDependency> SubpassDependencies;

		for (uint32_t AttachmentIdx = 0; AttachmentIdx < InCreateInfo.ColorAttachments.size(); AttachmentIdx++)
		{
			const auto& AttachmentInfo = InCreateInfo.ColorAttachments[AttachmentIdx];

			Image2DInfo ImageInfo = {};
			ImageInfo.Size = { InCreateInfo.Width, InCreateInfo.Height };
			ImageInfo.Usage = EImageUsage::Attachment;
			ImageInfo.Format = AttachmentInfo.Format;
			ImageInfo.Flags = AttachmentInfo.Flags;
			ImageInfo.InitialState = AttachmentInfo.InitialState;
			ImageInfo.ClearColor = AttachmentInfo.ClearColor;

			for (size_t ImageIdx = 0; ImageIdx < FrameCount; ImageIdx++)
			{
				uint32_t Idx = AttachmentIdx * InCreateInfo.ColorAttachments.size() + ImageIdx;

				Shared<VulkanImage2D> Image = Shared<VulkanImage2D>::Create(InDevice, ImageInfo);

				VkImageViewCreateInfo ViewInfo = {};
				ViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				ViewInfo.image = Image->GetImage();
				ViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				ViewInfo.format = EFormatToVkFormat(AttachmentInfo.Format);
				ViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				ViewInfo.subresourceRange.baseArrayLayer = 0;
				ViewInfo.subresourceRange.baseMipLevel = 0;
				ViewInfo.subresourceRange.layerCount = 1;
				ViewInfo.subresourceRange.levelCount = 1;

				vkCreateImageView(Device->GetLogicalDevice(), &ViewInfo, nullptr, &m_ImageViews[Idx]);

				m_Images[Idx] = Image;
			}

			auto& AttachmentDesc = AttachmentDescriptions[AttachmentIdx];
			AttachmentDesc.flags = 0;
			AttachmentDesc.format = EFormatToVkFormat(AttachmentInfo.Format);
			AttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
			AttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			AttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			AttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			AttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			AttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			AttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			AttachmentReferences[AttachmentIdx].attachment = AttachmentIdx;
			AttachmentReferences[AttachmentIdx].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			auto& Dependency = SubpassDependencies.emplace_back();
			Dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			Dependency.dstSubpass = 0;
			Dependency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			Dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			Dependency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			Dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			Dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
		}

		VkSubpassDescription SubpassDesc = {};
		SubpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		SubpassDesc.colorAttachmentCount = AttachmentReferences.size();
		SubpassDesc.pColorAttachments = AttachmentReferences.data();

		VkRenderPassCreateInfo RenderPassInfo = {};
		RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		RenderPassInfo.pNext = nullptr;
		RenderPassInfo.flags = 0;
		RenderPassInfo.attachmentCount = AttachmentDescriptions.size();
		RenderPassInfo.pAttachments = AttachmentDescriptions.data();
		RenderPassInfo.subpassCount = 1;
		RenderPassInfo.pSubpasses = &SubpassDesc;
		RenderPassInfo.dependencyCount = SubpassDependencies.size();
		RenderPassInfo.pDependencies = SubpassDependencies.data();

		vkCreateRenderPass(Device->GetLogicalDevice(), &RenderPassInfo, nullptr, &m_RenderPass);

		VkFramebufferCreateInfo FrameBufferInfo = {};
		FrameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		FrameBufferInfo.renderPass = m_RenderPass;
		FrameBufferInfo.width = m_CreateInfo.Width;
		FrameBufferInfo.height = m_CreateInfo.Height;
		FrameBufferInfo.layers = 1;
		FrameBufferInfo.attachmentCount = AttachmentCount;
		FrameBufferInfo.pAttachments = m_ImageViews.data();

		vkCreateFramebuffer(Device->GetLogicalDevice(), &FrameBufferInfo, nullptr, &m_FrameBuffer);
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