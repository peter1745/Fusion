#include "FusionPCH.hpp"
#include "VulkanRenderTexture.hpp"
#include "VulkanContext.hpp"

#include "Fusion/Renderer/Renderer.hpp"

#include <vulkan/vulkan.h>

namespace Fusion {

	VulkanRenderTexture::VulkanRenderTexture(const RenderTextureInfo& InCreateInfo)
	    : m_CreateInfo(InCreateInfo)
	{
		Invalidate();
	}

	void VulkanRenderTexture::Bind(CommandList* InCommandList)
	{
		auto CmdList = dynamic_cast<VulkanCommandList*>(InCommandList)->GetBuffer();

		VkExtent2D ImageExtent = { m_CreateInfo.Width, m_CreateInfo.Height };

		VkRenderPassBeginInfo RenderPassBeginInfo = {};
		RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		RenderPassBeginInfo.renderPass = m_RenderPass;
		RenderPassBeginInfo.framebuffer = m_FrameBuffer;
		RenderPassBeginInfo.renderArea.offset = { 0, 0 };
		RenderPassBeginInfo.renderArea.extent = ImageExtent;
		RenderPassBeginInfo.clearValueCount = m_ClearValues.size();
		RenderPassBeginInfo.pClearValues = m_ClearValues.data();

		vkCmdBeginRenderPass(CmdList, &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanRenderTexture::Unbind(CommandList* InCommandList)
	{
		auto CmdList = dynamic_cast<VulkanCommandList*>(InCommandList)->GetBuffer();
		vkCmdEndRenderPass(CmdList);

		// Update the state of the images to reflect their final layout
		for (auto Image : m_Images)
		{
			if (!Image)
				continue;

			if (IsDepthFormat(Image->m_CreateInfo.Format))
			{
				Image->m_State = ImageStates::DepthRead;
			}
			else
			{
				Image->m_State = ImageStates::PixelShaderResource;
			}
		}
	}

	void VulkanRenderTexture::Clear()
	{
	}

	void VulkanRenderTexture::Resize(uint32_t InAttachmentIndex, const ImageSize& InSize)
	{
		if (InSize.Width == 0 || InSize.Height == 0)
			return;

		m_CreateInfo.Width = InSize.Width;
		m_CreateInfo.Height = InSize.Height;

		Invalidate();
	}

	void VulkanRenderTexture::TransitionImages(CommandList* InCommandList, EImageState InColorAttachmentState, EImageState InDepthStencilState)
	{
		m_Images[0]->Transition(InCommandList, InColorAttachmentState);
		m_Images[m_Images.size() - 1]->Transition(InCommandList, InDepthStencilState);
	}

	void VulkanRenderTexture::Invalidate()
	{
		if (m_FrameBuffer != VK_NULL_HANDLE)
			Release();

		uint32_t AttachmentCount = m_CreateInfo.ColorAttachments.size() + (IsDepthFormat(m_CreateInfo.DepthAttachment.Format) ? 1 : 0);

		auto Device = GraphicsContext::Get<VulkanContext>()->GetDevice().As<VulkanDevice>();

		m_Images.resize(AttachmentCount);
		m_ImageViews.resize(AttachmentCount);

		m_ClearValues.resize(AttachmentCount);

		std::vector<VkAttachmentDescription> AttachmentDescriptions(m_CreateInfo.ColorAttachments.size());
		std::vector<VkAttachmentReference> AttachmentReferences(m_CreateInfo.ColorAttachments.size());
		std::vector<VkSubpassDependency> SubpassDependencies;

		for (uint32_t AttachmentIdx = 0; AttachmentIdx < m_CreateInfo.ColorAttachments.size(); AttachmentIdx++)
		{
			const auto& AttachmentInfo = m_CreateInfo.ColorAttachments[AttachmentIdx];

			Image2DInfo ImageInfo = {};
			ImageInfo.Size = { m_CreateInfo.Width, m_CreateInfo.Height };
			ImageInfo.Usage = EImageUsage::Attachment;
			ImageInfo.Format = AttachmentInfo.Format;
			ImageInfo.Flags = AttachmentInfo.Flags;
			ImageInfo.InitialState = ImageStates::Undefined;
			ImageInfo.ClearColor = AttachmentInfo.ClearColor;

			Shared<VulkanImage2D> Image = Shared<VulkanImage2D>::Create(Device, ImageInfo);

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

			vkCreateImageView(Device->GetLogicalDevice(), &ViewInfo, nullptr, &m_ImageViews[AttachmentIdx]);

			m_Images[AttachmentIdx] = Image;

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

			auto& Dependency1 = SubpassDependencies.emplace_back();
			Dependency1.srcSubpass = 0;
			Dependency1.dstSubpass = VK_SUBPASS_EXTERNAL;
			Dependency1.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			Dependency1.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			Dependency1.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			Dependency1.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			Dependency1.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			m_ClearValues[AttachmentIdx].color.float32[0] = AttachmentInfo.ClearColor.r;
			m_ClearValues[AttachmentIdx].color.float32[1] = AttachmentInfo.ClearColor.g;
			m_ClearValues[AttachmentIdx].color.float32[2] = AttachmentInfo.ClearColor.b;
			m_ClearValues[AttachmentIdx].color.float32[3] = AttachmentInfo.ClearColor.a;
		}

		VkAttachmentReference DepthAttachmentReference;

		if (IsDepthFormat(m_CreateInfo.DepthAttachment.Format))
		{
			Image2DInfo ImageInfo = {};
			ImageInfo.Size = { m_CreateInfo.Width, m_CreateInfo.Height };
			ImageInfo.Usage = EImageUsage::Attachment;
			ImageInfo.Format = m_CreateInfo.DepthAttachment.Format;
			ImageInfo.Flags = m_CreateInfo.DepthAttachment.Flags;
			ImageInfo.InitialState = ImageStates::Undefined;
			ImageInfo.ClearColor = m_CreateInfo.DepthAttachment.ClearColor;

			Shared<VulkanImage2D> Image = Shared<VulkanImage2D>::Create(Device, ImageInfo);

			VkImageViewCreateInfo ViewInfo = {};
			ViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			ViewInfo.image = Image->GetImage();
			ViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			ViewInfo.format = EFormatToVkFormat(m_CreateInfo.DepthAttachment.Format);
			ViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			ViewInfo.subresourceRange.baseArrayLayer = 0;
			ViewInfo.subresourceRange.baseMipLevel = 0;
			ViewInfo.subresourceRange.layerCount = 1;
			ViewInfo.subresourceRange.levelCount = 1;

			vkCreateImageView(Device->GetLogicalDevice(), &ViewInfo, nullptr, &m_ImageViews[m_ImageViews.size() - 1]);

			m_Images.push_back(Image);

			auto& AttachmentDesc = AttachmentDescriptions.emplace_back();
			AttachmentDesc.flags = 0;
			AttachmentDesc.format = EFormatToVkFormat(m_CreateInfo.DepthAttachment.Format);
			AttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
			AttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			AttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			AttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			AttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			AttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			AttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

			DepthAttachmentReference.attachment = AttachmentDescriptions.size() - 1;
			DepthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			m_ClearValues[AttachmentCount - 1].color.float32[0] = m_CreateInfo.DepthAttachment.ClearColor.r;
			m_ClearValues[AttachmentCount - 1].color.float32[1] = m_CreateInfo.DepthAttachment.ClearColor.g;
			m_ClearValues[AttachmentCount - 1].color.float32[2] = m_CreateInfo.DepthAttachment.ClearColor.b;
			m_ClearValues[AttachmentCount - 1].color.float32[3] = m_CreateInfo.DepthAttachment.ClearColor.a;
		}

		VkSubpassDescription SubpassDesc = {};
		SubpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		SubpassDesc.colorAttachmentCount = AttachmentReferences.size();
		SubpassDesc.pColorAttachments = AttachmentReferences.data();

		if (IsDepthFormat(m_CreateInfo.DepthAttachment.Format))
			SubpassDesc.pDepthStencilAttachment = &DepthAttachmentReference;

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
		FrameBufferInfo.attachmentCount = m_ImageViews.size();
		FrameBufferInfo.pAttachments = m_ImageViews.data();

		vkCreateFramebuffer(Device->GetLogicalDevice(), &FrameBufferInfo, nullptr, &m_FrameBuffer);
	}

	void VulkanRenderTexture::Release()
	{
		if (m_FrameBuffer == VK_NULL_HANDLE)
			return;

		Renderer::GetCurrent().SubmitResourceForDestruction(3, [InFramebuffer = m_FrameBuffer, InRenderPass = m_RenderPass, InImageViews = m_ImageViews]() {
			auto Device = GraphicsContext::Get<VulkanContext>()->GetDevice().As<VulkanDevice>();

			vkDestroyFramebuffer(Device->GetLogicalDevice(), InFramebuffer, nullptr);
			vkDestroyRenderPass(Device->GetLogicalDevice(), InRenderPass, nullptr);

			for (auto& ImageView : InImageViews)
				vkDestroyImageView(Device->GetLogicalDevice(), ImageView, nullptr);
		});

		for (auto Image : m_Images)
		{
			if (!Image)
				continue;

			Image->Release();
		}
	}
}