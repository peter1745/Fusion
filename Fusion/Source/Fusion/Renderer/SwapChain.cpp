#include "FusionPCH.hpp"
#include "SwapChain.hpp"
#include "VulkanFunctions.hpp"

#include "Fusion/Core/Application.hpp"

#include <spdlog/fmt/fmt.h>
#include <GLFW/glfw3.h>

namespace Fusion {

	SwapChain::SwapChain(const Shared<GraphicsContext>& InContext, const SwapChainInfo& InCreateInfo)
	    : m_Context(InContext), m_CreateInfo(InCreateInfo)
	{
		Create(false);
	}

	void SwapChain::Bind(CommandBuffer* InCommandList)
	{
		VkViewport Viewport = {};
		Viewport.x = 0.0f;
		Viewport.y = 0.0f;
		Viewport.width = float(m_CreateInfo.Width);
		Viewport.height = float(m_CreateInfo.Height);
		Viewport.minDepth = 0.0f;
		Viewport.maxDepth = 0.0f;
		vkCmdSetViewport(InCommandList->GetBuffer(), 0, 1, &Viewport);

		VkRect2D Scissor = {};
		Scissor.offset = { 0, 0 };
		Scissor.extent = m_ImageExtent;
		vkCmdSetScissor(InCommandList->GetBuffer(), 0, 1, &Scissor);

		ImageTransitionInfo TransitionInfo = {};
		TransitionInfo.Image = m_Images[m_CurrentImage];
		TransitionInfo.OldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		TransitionInfo.NewLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		TransitionInfo.SrcAccessFlag = 0;
		TransitionInfo.DstAccessFlag = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		TransitionInfo.SrcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		TransitionInfo.DstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		TransitionInfo.AspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
		TransitionImage(InCommandList->GetBuffer(), TransitionInfo);

		VkClearValue ClearValue = {};
		ClearValue.color.float32[0] = 0.25f;
		ClearValue.color.float32[1] = 0.0f;
		ClearValue.color.float32[2] = 0.0f;
		ClearValue.color.float32[3] = 1.0f;

		VkRenderingAttachmentInfoKHR ColorAttachment = {};
		ColorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
		ColorAttachment.imageView = m_ImageViews[m_CurrentImage];
		ColorAttachment.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
		ColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		ColorAttachment.clearValue = ClearValue;

		VkRenderingInfoKHR RenderingInfo = {};
		RenderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
		RenderingInfo.renderArea.offset = { 0, 0 };
		RenderingInfo.renderArea.extent = m_ImageExtent;
		RenderingInfo.layerCount = 1;
		RenderingInfo.viewMask = 0;
		RenderingInfo.colorAttachmentCount = 1;
		RenderingInfo.pColorAttachments = &ColorAttachment;
		RenderingInfo.pDepthAttachment = nullptr;
		evkCmdBeginRenderingKHR(InCommandList->GetBuffer(), &RenderingInfo);
	}

	void SwapChain::Clear(CommandBuffer* InCommandList) {}

	void SwapChain::Present(VkSemaphore InFinishedSemaphore)
	{
		VkPresentInfoKHR PresentInfo = {};
		PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		PresentInfo.waitSemaphoreCount = 1;
		PresentInfo.pWaitSemaphores = &InFinishedSemaphore;
		PresentInfo.swapchainCount = 1;
		PresentInfo.pSwapchains = &m_SwapChain;
		PresentInfo.pImageIndices = &m_CurrentImage;
		VkResult Result = vkQueuePresentKHR(m_Context->GetDevice()->GetQueueInfo().Queue, &PresentInfo);

		if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR)
		{
			Invalidate();
		}
		else if (Result != VK_SUCCESS)
		{
			FUSION_CORE_VERIFY(false, "Error trying to present to the screen.");
		}
	}

	void SwapChain::Unbind(CommandBuffer* InCommandList)
	{
		auto CmdList = dynamic_cast<CommandBuffer*>(InCommandList)->GetBuffer();
		evkCmdEndRenderingKHR(CmdList);
	}

	void SwapChain::Resize(uint32_t InWidth, uint32_t InHeight)
	{
	}

	void SwapChain::Create(bool InWasInvalidated)
	{
		SurfaceProperties SurfaceProps = m_Context->GetDevice()->GetSurfaceProperties(m_Context->GetSurface());

		VkSurfaceFormatKHR Format = SurfaceProps.Formats[0];
		if (Format.format != VK_FORMAT_B8G8R8A8_UNORM || Format.colorSpace != VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			// Find suitable format

			for (uint32_t Idx = 1; Idx < SurfaceProps.Formats.size(); Idx++)
			{
				Format = SurfaceProps.Formats[Idx];

				if (Format.format == VK_FORMAT_B8G8R8A8_UNORM)
					break;
			}

			if (Format.format != VK_FORMAT_B8G8R8A8_UNORM)
				Format = SurfaceProps.Formats[0]; // Just pick the first if the desired one isn't found
		}

		m_ImageFormat = Format.format;

		// Find suitable present mode
		VkPresentModeKHR PresentMode = SurfaceProps.PresentModes[0];
		if (PresentMode != VK_PRESENT_MODE_MAILBOX_KHR)
		{
			for (uint32_t Idx = 1; Idx < SurfaceProps.PresentModes.size(); Idx++)
			{
				PresentMode = SurfaceProps.PresentModes[Idx];
				if (PresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
					break;
			}

			// Look for FIFO if mailbox isn't supported
			if (PresentMode != VK_PRESENT_MODE_MAILBOX_KHR)
			{
				for (uint32_t Idx = 1; Idx < SurfaceProps.PresentModes.size(); Idx++)
				{
					PresentMode = SurfaceProps.PresentModes[Idx];
					if (PresentMode == VK_PRESENT_MODE_FIFO_KHR)
						break;
				}
			}
		}

		if (SurfaceProps.Capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			m_ImageExtent = SurfaceProps.Capabilities.currentExtent;
		}
		else
		{
			auto* NativeWindow = static_cast<GLFWwindow*>(Application::Get().GetWindow()->GetWindowHandle());
			int32_t ImageWidth = 0, ImageHeight = 0;
			glfwGetFramebufferSize(NativeWindow, &ImageWidth, &ImageHeight);
			m_ImageExtent.width = std::clamp(uint32_t(ImageWidth), SurfaceProps.Capabilities.minImageExtent.width, SurfaceProps.Capabilities.maxImageExtent.width);
			m_ImageExtent.height = std::clamp(uint32_t(ImageHeight), SurfaceProps.Capabilities.minImageExtent.height, SurfaceProps.Capabilities.maxImageExtent.height);
		}

		m_ImageCount = std::min(SurfaceProps.Capabilities.minImageCount + 1, SurfaceProps.Capabilities.maxImageCount);

		VkSwapchainCreateInfoKHR SwapchainInfo = {};
		SwapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		SwapchainInfo.surface = m_Context->GetSurface();
		SwapchainInfo.minImageCount = m_ImageCount;
		SwapchainInfo.imageFormat = Format.format;
		SwapchainInfo.imageColorSpace = Format.colorSpace;
		SwapchainInfo.imageExtent = m_ImageExtent;
		SwapchainInfo.imageArrayLayers = 1;
		SwapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		SwapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		SwapchainInfo.queueFamilyIndexCount = 0;
		SwapchainInfo.pQueueFamilyIndices = nullptr;
		SwapchainInfo.preTransform = SurfaceProps.Capabilities.currentTransform;
		SwapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		SwapchainInfo.presentMode = PresentMode;
		SwapchainInfo.clipped = VK_TRUE;
		SwapchainInfo.oldSwapchain = VK_NULL_HANDLE;

		FUSION_CORE_VERIFY(vkCreateSwapchainKHR(m_Context->GetDevice()->GetLogicalDevice(), &SwapchainInfo, nullptr, &m_SwapChain) == VK_SUCCESS);

		// Get SwapChain images and create image views
		{
			FUSION_CORE_VERIFY(vkGetSwapchainImagesKHR(m_Context->GetDevice()->GetLogicalDevice(), m_SwapChain, &m_ImageCount, nullptr) == VK_SUCCESS);
			m_Images.resize(m_ImageCount);
			m_ImageViews.resize(m_ImageCount);
			FUSION_CORE_VERIFY(vkGetSwapchainImagesKHR(m_Context->GetDevice()->GetLogicalDevice(), m_SwapChain, &m_ImageCount, m_Images.data()) == VK_SUCCESS);

			for (uint32_t Idx = 0; Idx < m_ImageCount; Idx++)
			{
				VkImageViewCreateInfo ImageViewInfo = {};
				ImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				ImageViewInfo.image = m_Images[Idx];
				ImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				ImageViewInfo.format = Format.format;

				ImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				ImageViewInfo.subresourceRange.levelCount = 1;
				ImageViewInfo.subresourceRange.baseMipLevel = 0;
				ImageViewInfo.subresourceRange.layerCount = 1;
				ImageViewInfo.subresourceRange.baseArrayLayer = 0;

				FUSION_CORE_VERIFY(vkCreateImageView(m_Context->GetDevice()->GetLogicalDevice(), &ImageViewInfo, nullptr, &m_ImageViews[Idx]) == VK_SUCCESS);
			}
		}

		if (InWasInvalidated)
			m_OnInvalidatedCallbacks.Invoke(*this);
	}

	void SwapChain::Invalidate()
	{
		m_Context->GetDevice()->Wait();

		for (auto& View : m_ImageViews)
			vkDestroyImageView(m_Context->GetDevice()->GetLogicalDevice(), View, nullptr);

		vkDestroySwapchainKHR(m_Context->GetDevice()->GetLogicalDevice(), m_SwapChain, nullptr);

		m_CurrentImage = 0;

		Create(true);
	}

	bool SwapChain::AcquireNextImage(VkDevice InDevice, VkSemaphore InImageAvailableSemaphore)
	{
		VkResult Result = vkAcquireNextImageKHR(InDevice, m_SwapChain, UINT64_MAX, InImageAvailableSemaphore, VK_NULL_HANDLE, &m_CurrentImage);

		if (Result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			Invalidate();
			return false;
		}

		FUSION_CORE_VERIFY(Result == VK_SUCCESS);
		return true;
	}

	void SwapChain::Release()
	{
		// Ensure the queue isn't busy before we try to destroy the resources
		vkQueueWaitIdle(m_Context->GetDevice()->GetQueueInfo().Queue);

		for (auto& View : m_ImageViews)
			vkDestroyImageView(m_Context->GetDevice()->GetLogicalDevice(), View, nullptr);

		vkDestroySwapchainKHR(m_Context->GetDevice()->GetLogicalDevice(), m_SwapChain, nullptr);
	}

}