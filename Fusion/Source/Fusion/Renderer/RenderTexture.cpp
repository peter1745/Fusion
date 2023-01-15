#include "FusionPCH.hpp"
#include "RenderTexture.hpp"
#include "GraphicsContext.hpp"
#include "VulkanFunctions.hpp"

#include "Fusion/Renderer/Renderer.hpp"

#include <vulkan/vulkan.h>

namespace Fusion {

	RenderTexture::RenderTexture(const RenderTextureInfo& InCreateInfo)
	    : m_CreateInfo(InCreateInfo)
	{
		Invalidate();
	}

	void RenderTexture::Bind(CommandBuffer* InCommandList)
	{
		VkExtent2D ImageExtent = { m_CreateInfo.Width, m_CreateInfo.Height };

		for (uint32_t Idx = 0; Idx < m_CreateInfo.ColorAttachments.size(); Idx++)
		{
			ImageTransitionInfo TransitionInfo = {};
			TransitionInfo.Image = m_Images[Idx]->GetImage();
			TransitionInfo.OldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			TransitionInfo.NewLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			TransitionInfo.SrcAccessFlag = 0;
			TransitionInfo.DstAccessFlag = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			TransitionInfo.SrcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			TransitionInfo.DstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			TransitionInfo.AspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;

			TransitionImage(InCommandList->GetBuffer(), TransitionInfo);

			m_Images[Idx]->m_State = EImageState::ColorAttachmentOptimal;
		}

		if (IsDepthFormat(m_CreateInfo.DepthAttachment.Format))
		{
			ImageTransitionInfo TransitionInfo = {};
			TransitionInfo.Image = m_Images[m_Images.size() - 1]->GetImage();
			TransitionInfo.OldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			TransitionInfo.NewLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			TransitionInfo.SrcAccessFlag = 0;
			TransitionInfo.DstAccessFlag = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			TransitionInfo.SrcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			TransitionInfo.DstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			TransitionInfo.AspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			TransitionImage(InCommandList->GetBuffer(), TransitionInfo);
		}

		std::vector<VkRenderingAttachmentInfoKHR> ColorAttachments(m_CreateInfo.ColorAttachments.size());
		for (size_t Idx = 0; Idx < m_CreateInfo.ColorAttachments.size(); Idx++)
		{
			auto& Attachment = ColorAttachments[Idx];
			Attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
			Attachment.imageView = m_ImageViews[Idx];
			Attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			Attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			Attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			Attachment.clearValue = m_ClearValues[Idx];
		}

		VkRenderingAttachmentInfoKHR DepthAttachment = {};
		DepthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
		
		bool HasDepthAttachment = IsDepthFormat(m_CreateInfo.DepthAttachment.Format);
		if (HasDepthAttachment)
		{
			DepthAttachment.imageView = m_ImageViews[m_ImageViews.size() - 1];
			DepthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			DepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			DepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			DepthAttachment.clearValue = m_ClearValues[m_ClearValues.size() - 1];
		}

		VkRenderingInfoKHR RenderingInfo = {};
		RenderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
		RenderingInfo.renderArea.offset = { 0, 0 };
		RenderingInfo.renderArea.extent = ImageExtent;
		RenderingInfo.layerCount = 1;
		RenderingInfo.viewMask = 0;
		RenderingInfo.colorAttachmentCount = ColorAttachments.size();
		RenderingInfo.pColorAttachments = ColorAttachments.data();
		RenderingInfo.pDepthAttachment = HasDepthAttachment ? &DepthAttachment : nullptr;
		evkCmdBeginRenderingKHR(InCommandList->GetBuffer(), &RenderingInfo);
	}

	void RenderTexture::Unbind(CommandBuffer* InCommandList)
	{
		evkCmdEndRenderingKHR(InCommandList->GetBuffer());

		for (uint32_t Idx = 0; Idx < m_CreateInfo.ColorAttachments.size(); Idx++)
		{
			ImageTransitionInfo TransitionInfo = {};
			TransitionInfo.Image = m_Images[Idx]->GetImage();
			TransitionInfo.OldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			TransitionInfo.NewLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			TransitionInfo.SrcAccessFlag = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			TransitionInfo.DstAccessFlag = 0;
			TransitionInfo.SrcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			TransitionInfo.DstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			TransitionInfo.AspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
			TransitionImage(InCommandList->GetBuffer(), TransitionInfo);

			m_Images[Idx]->m_State = EImageState::PixelShaderResource;
		}
	}

	void RenderTexture::Clear() {}

	void RenderTexture::Resize(uint32_t InAttachmentIndex, const ImageSize& InSize)
	{
		if (InSize.Width == 0 || InSize.Height == 0)
			return;

		m_CreateInfo.Width = InSize.Width;
		m_CreateInfo.Height = InSize.Height;

		Invalidate();
	}

	void RenderTexture::TransitionImages(CommandBuffer* InCommandList, EImageState InColorAttachmentState, EImageState InDepthStencilState)
	{
		//m_Images[0]->Transition(InCommandList, InColorAttachmentState);
		//m_Images[m_Images.size() - 1]->Transition(InCommandList, InDepthStencilState);
	}

	void RenderTexture::Invalidate()
	{
		Release();

		uint32_t AttachmentCount = m_CreateInfo.ColorAttachments.size() + (IsDepthFormat(m_CreateInfo.DepthAttachment.Format) ? 1 : 0);

		m_Images.resize(AttachmentCount);
		m_ImageViews.resize(AttachmentCount);

		m_ClearValues.resize(AttachmentCount);

		for (uint32_t AttachmentIdx = 0; AttachmentIdx < m_CreateInfo.ColorAttachments.size(); AttachmentIdx++)
		{
			const auto& AttachmentInfo = m_CreateInfo.ColorAttachments[AttachmentIdx];

			Image2DInfo ImageInfo = {};
			ImageInfo.Size = { m_CreateInfo.Width, m_CreateInfo.Height };
			ImageInfo.Usage = EImageUsage::Attachment;
			ImageInfo.Format = AttachmentInfo.Format;
			ImageInfo.Flags = AttachmentInfo.Flags;
			ImageInfo.InitialState = EImageState::Undefined;
			ImageInfo.ClearColor = AttachmentInfo.ClearColor;

			if (AttachmentInfo.IsMappable)
				ImageInfo.Usage |= EImageUsage::CopySource;

			Shared<Image2D> Image = Shared<Image2D>::Create(ImageInfo);

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

			vkCreateImageView(GraphicsContext::Get()->GetDevice()->GetLogicalDevice(), &ViewInfo, nullptr, &m_ImageViews[AttachmentIdx]);

			m_Images[AttachmentIdx] = Image;

			m_ClearValues[AttachmentIdx].color.float32[0] = AttachmentInfo.ClearColor.r;
			m_ClearValues[AttachmentIdx].color.float32[1] = AttachmentInfo.ClearColor.g;
			m_ClearValues[AttachmentIdx].color.float32[2] = AttachmentInfo.ClearColor.b;
			m_ClearValues[AttachmentIdx].color.float32[3] = AttachmentInfo.ClearColor.a;
		}

		if (IsDepthFormat(m_CreateInfo.DepthAttachment.Format))
		{
			Image2DInfo ImageInfo = {};
			ImageInfo.Size = { m_CreateInfo.Width, m_CreateInfo.Height };
			ImageInfo.Usage = EImageUsage::Attachment;
			ImageInfo.Format = m_CreateInfo.DepthAttachment.Format;
			ImageInfo.Flags = m_CreateInfo.DepthAttachment.Flags;
			ImageInfo.InitialState = EImageState::Undefined;
			ImageInfo.ClearColor = m_CreateInfo.DepthAttachment.ClearColor;

			Shared<Image2D> Image = Shared<Image2D>::Create(ImageInfo);

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

			vkCreateImageView(GraphicsContext::Get()->GetDevice()->GetLogicalDevice(), &ViewInfo, nullptr, &m_ImageViews[m_ImageViews.size() - 1]);

			m_Images.push_back(Image);

			m_ClearValues[AttachmentCount - 1].depthStencil.depth = 1.0f;
		}
	}

	void RenderTexture::Release()
	{
		/*Renderer::GetCurrent().SubmitResourceForDestruction(3, [InImageViews = m_ImageViews]() {
			auto Device = GraphicsContext::Get<GraphicsContext>()->GetDevice().As<Device>();

			for (auto& ImageView : InImageViews)
				vkDestroyImageView(Device->GetLogicalDevice(), ImageView, nullptr);
		});*/

		for (auto Image : m_Images)
		{
			if (!Image)
				continue;

			Image->Release();
		}
	}
}