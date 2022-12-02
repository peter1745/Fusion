#include "VulkanSwapChain.hpp"
#include "VulkanContext.hpp"
#include "VulkanFunctions.hpp"

#include "Fusion/Core/Application.hpp"

#include <spdlog/fmt/fmt.h>
#include <GLFW/glfw3.h>

namespace Fusion {

	VulkanSwapChain::VulkanSwapChain(const SwapChainInfo& InCreateInfo)
	    : m_CreateInfo(InCreateInfo)
	{
		Create(false);
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
	}

	void VulkanSwapChain::Bind()
	{
		auto Ctx = GraphicsContext::Get<VulkanContext>();
		auto CmdList = static_cast<VulkanCommandList*>(Ctx->GetCurrentCommandList())->GetBuffer();

		VkViewport Viewport = {};
		Viewport.x = 0.0f;
		Viewport.y = 0.0f;
		Viewport.width = float(m_CreateInfo.Width);
		Viewport.height = float(m_CreateInfo.Height);
		Viewport.minDepth = 0.0f;
		Viewport.maxDepth = 0.0f;
		vkCmdSetViewport(CmdList, 0, 1, &Viewport);

		VkRect2D Scissor = {};
		Scissor.offset = { 0, 0 };
		Scissor.extent = m_ImageExtent;
		vkCmdSetScissor(CmdList, 0, 1, &Scissor);

		VkClearValue ClearValue = {};
		ClearValue.color.float32[0] = 0.25f;
		ClearValue.color.float32[1] = 0.0f;
		ClearValue.color.float32[2] = 0.0f;
		ClearValue.color.float32[3] = 1.0f;

		VkRenderPassBeginInfo RenderPassBeginInfo = {};
		RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		RenderPassBeginInfo.renderPass = m_RenderPass;
		RenderPassBeginInfo.framebuffer = m_Framebuffers[m_CurrentImage];
		RenderPassBeginInfo.renderArea.offset = { 0, 0 };
		RenderPassBeginInfo.renderArea.extent = m_ImageExtent;
		RenderPassBeginInfo.clearValueCount = 1;
		RenderPassBeginInfo.pClearValues = &ClearValue;

		vkCmdBeginRenderPass(CmdList, &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanSwapChain::Clear() {}

	void VulkanSwapChain::Present()
	{
		VkPresentInfoKHR PresentInfo = {};
		PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		PresentInfo.waitSemaphoreCount = 1;
		PresentInfo.pWaitSemaphores = &m_RenderFinishedSemaphores[m_CurrentFrame];
		PresentInfo.swapchainCount = 1;
		PresentInfo.pSwapchains = &m_SwapChain;
		PresentInfo.pImageIndices = &m_CurrentImage;
		VkResult Result = vkQueuePresentKHR(GraphicsContext::Get<VulkanContext>()->GetQueueInfo().Queue, &PresentInfo);

		if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR)
		{
			Invalidate();
		}
		else if (Result != VK_SUCCESS)
		{
			FUSION_CORE_VERIFY(false, "Error trying to present to the screen.");
		}

		m_CurrentFrame = (m_CurrentFrame + 1) % m_FrameCount;
	}

	void VulkanSwapChain::Unbind()
	{
		auto Ctx = GraphicsContext::Get<VulkanContext>();
		auto CmdList = static_cast<VulkanCommandList*>(Ctx->GetCurrentCommandList())->GetBuffer();

		vkCmdEndRenderPass(CmdList);
	}

	void VulkanSwapChain::Resize(uint32_t InWidth, uint32_t InHeight)
	{
	}

	void VulkanSwapChain::Create(bool InWasInvalidated)
	{
		auto Context = GraphicsContext::Get<VulkanContext>();

		InitSurface();

		VkSwapchainCreateInfoKHR SwapchainInfo = {};
		SwapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		SwapchainInfo.surface = Context->GetSurface();
		SwapchainInfo.minImageCount = m_ImageCount;
		SwapchainInfo.imageFormat = m_SurfaceFormat.format;
		SwapchainInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
		SwapchainInfo.imageExtent = m_ImageExtent;
		SwapchainInfo.imageArrayLayers = 1;
		SwapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		SwapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		SwapchainInfo.queueFamilyIndexCount = 0;
		SwapchainInfo.pQueueFamilyIndices = nullptr;
		SwapchainInfo.preTransform = m_SurfaceCapabilities.currentTransform;
		SwapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		SwapchainInfo.presentMode = m_PresentMode;
		SwapchainInfo.clipped = VK_TRUE;
		SwapchainInfo.oldSwapchain = VK_NULL_HANDLE;

		FUSION_CORE_VERIFY(vkCreateSwapchainKHR(Context->GetDevice(), &SwapchainInfo, nullptr, &m_SwapChain) == VK_SUCCESS);

		// Get SwapChain images and create image views
		{
			FUSION_CORE_VERIFY(vkGetSwapchainImagesKHR(Context->GetDevice(), m_SwapChain, &m_ImageCount, nullptr) == VK_SUCCESS);
			m_Images.resize(m_ImageCount);
			m_ImageViews.resize(m_ImageCount);
			FUSION_CORE_VERIFY(vkGetSwapchainImagesKHR(Context->GetDevice(), m_SwapChain, &m_ImageCount, m_Images.data()) == VK_SUCCESS);

			for (uint32_t Idx = 0; Idx < m_ImageCount; Idx++)
			{
				VkImageViewCreateInfo ImageViewInfo = {};
				ImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				ImageViewInfo.image = m_Images[Idx];
				ImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				ImageViewInfo.format = m_SurfaceFormat.format;

				ImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				ImageViewInfo.subresourceRange.levelCount = 1;
				ImageViewInfo.subresourceRange.baseMipLevel = 0;
				ImageViewInfo.subresourceRange.layerCount = 1;
				ImageViewInfo.subresourceRange.baseArrayLayer = 0;

				FUSION_CORE_VERIFY(vkCreateImageView(Context->GetDevice(), &ImageViewInfo, nullptr, &m_ImageViews[Idx]) == VK_SUCCESS);
			}
		}

		m_FrameCount = std::min(m_FrameCount, m_SurfaceCapabilities.maxImageCount);

		// Create semaphores and fences
		{
			m_ImageAvailableSemaphores.resize(m_ImageCount);
			m_RenderFinishedSemaphores.resize(m_ImageCount);
			m_ImageFences.resize(m_ImageCount);

			VkSemaphoreCreateInfo SemaphoreInfo = {};
			SemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			VkFenceCreateInfo FenceInfo = {};
			FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			FenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			for (uint32_t Idx = 0; Idx < m_ImageCount; Idx++)
			{
				vkCreateSemaphore(Context->GetDevice(), &SemaphoreInfo, nullptr, &m_ImageAvailableSemaphores[Idx]);
				vkCreateSemaphore(Context->GetDevice(), &SemaphoreInfo, nullptr, &m_RenderFinishedSemaphores[Idx]);
				vkCreateFence(Context->GetDevice(), &FenceInfo, nullptr, &m_ImageFences[Idx]);
			}
		}

		VkAttachmentDescription AttachmentInfo = {};
		AttachmentInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		AttachmentInfo.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		AttachmentInfo.format = m_SurfaceFormat.format;
		AttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		AttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		AttachmentInfo.samples = VK_SAMPLE_COUNT_1_BIT;

		VkAttachmentReference ColorAttachmentReference = {};
		ColorAttachmentReference.attachment = 0;
		ColorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription SubPassInfo = {};
		SubPassInfo.colorAttachmentCount = 1;
		SubPassInfo.pColorAttachments = &ColorAttachmentReference;
		SubPassInfo.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

		VkRenderPassCreateInfo RenderPassInfo = {};
		RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		RenderPassInfo.attachmentCount = 1;
		RenderPassInfo.pAttachments = &AttachmentInfo;
		RenderPassInfo.pSubpasses = &SubPassInfo;
		RenderPassInfo.subpassCount = 1;

		FUSION_CORE_VERIFY(vkCreateRenderPass(Context->GetDevice(), &RenderPassInfo, nullptr, &m_RenderPass) == VK_SUCCESS);

		m_Framebuffers.resize(m_ImageCount);
		for (size_t Idx = 0; Idx < m_ImageCount; Idx++)
		{
			VkFramebufferCreateInfo FramebufferInfo = {};
			FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			FramebufferInfo.renderPass = m_RenderPass;
			FramebufferInfo.width = m_ImageExtent.width;
			FramebufferInfo.height = m_ImageExtent.height;
			FramebufferInfo.layers = 1;
			FramebufferInfo.attachmentCount = 1;
			FramebufferInfo.pAttachments = &m_ImageViews[Idx];
			FUSION_CORE_VERIFY(vkCreateFramebuffer(Context->GetDevice(), &FramebufferInfo, nullptr, &m_Framebuffers[Idx]) == VK_SUCCESS);
		}
	}

	void VulkanSwapChain::InitSurface()
	{
		auto Context = GraphicsContext::Get<VulkanContext>();

		// Select a surface format
		{
			uint32_t NumFormats = 0;
			vkGetPhysicalDeviceSurfaceFormatsKHR(Context->GetPhysicalDevice(), Context->GetSurface(), &NumFormats, nullptr);
			std::vector<VkSurfaceFormatKHR> SupportedFormats(NumFormats);
			vkGetPhysicalDeviceSurfaceFormatsKHR(Context->GetPhysicalDevice(), Context->GetSurface(), &NumFormats, SupportedFormats.data());

			bool FoundSRGBFormat = false;
			for (const auto& Format : SupportedFormats)
			{
				if (Format.colorSpace != VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
					continue;

				if (Format.format == VK_FORMAT_B8G8R8A8_SRGB || Format.format == VK_FORMAT_R8G8B8A8_SRGB)
				{
					FoundSRGBFormat = true;
					m_SurfaceFormat = Format;
					break;
				}
			}

			if (!FoundSRGBFormat)
				m_SurfaceFormat = SupportedFormats[0];
		}

		// Find a present mode
		{
			uint32_t NumPresentModes = 0;
			vkGetPhysicalDeviceSurfacePresentModesKHR(Context->GetPhysicalDevice(), Context->GetSurface(), &NumPresentModes, nullptr);
			std::vector<VkPresentModeKHR> SupportedPresentModes(NumPresentModes);
			vkGetPhysicalDeviceSurfacePresentModesKHR(Context->GetPhysicalDevice(), Context->GetSurface(), &NumPresentModes, SupportedPresentModes.data());

			for (const auto& PresentMode : SupportedPresentModes)
			{
				if (PresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					// NOTE(Peter): We'll use VK_PRESENT_MODE_FIFO_KHR if we can't find a Mailbox present mode
					m_PresentMode = PresentMode;
					break;
				}
			}
		}

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Context->GetPhysicalDevice(), Context->GetSurface(), &m_SurfaceCapabilities);

		// Create Image Extent
		{
			if (m_SurfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			{
				m_ImageExtent = m_SurfaceCapabilities.currentExtent;
			}
			else
			{
				GLFWwindow* NativeWindow = static_cast<GLFWwindow*>(Application::Get().GetWindow()->GetWindowHandle());
				int32_t ImageWidth = 0, ImageHeight = 0;
				glfwGetFramebufferSize(NativeWindow, &ImageWidth, &ImageHeight);
				m_ImageExtent.width = std::clamp(uint32_t(ImageWidth), m_SurfaceCapabilities.minImageExtent.width, m_SurfaceCapabilities.maxImageExtent.width);
				m_ImageExtent.height = std::clamp(uint32_t(ImageHeight), m_SurfaceCapabilities.minImageExtent.height, m_SurfaceCapabilities.maxImageExtent.height);
			}
		}

		// Get image count
		m_ImageCount = std::min(m_SurfaceCapabilities.minImageCount + 1, m_SurfaceCapabilities.maxImageCount);
	}

	void VulkanSwapChain::Invalidate()
	{
		auto Context = GraphicsContext::Get<VulkanContext>();

		Context->WaitForGPU();

		for (size_t Idx = 0; Idx < m_Framebuffers.size(); Idx++)
			vkDestroyFramebuffer(Context->GetDevice(), m_Framebuffers[Idx], nullptr);

		vkDestroyRenderPass(Context->GetDevice(), m_RenderPass, nullptr);

		for (size_t Idx = 0; Idx < m_ImageViews.size(); Idx++)
			vkDestroyImageView(Context->GetDevice(), m_ImageViews[Idx], nullptr);
		vkDestroySwapchainKHR(Context->GetDevice(), m_SwapChain, nullptr);

		for (size_t Idx = 0; Idx < m_ImageAvailableSemaphores.size(); Idx++)
			vkDestroySemaphore(Context->GetDevice(), m_ImageAvailableSemaphores[Idx], nullptr);

		for (size_t Idx = 0; Idx < m_RenderFinishedSemaphores.size(); Idx++)
			vkDestroySemaphore(Context->GetDevice(), m_RenderFinishedSemaphores[Idx], nullptr);

		for (size_t Idx = 0; Idx < m_ImageFences.size(); Idx++)
			vkDestroyFence(Context->GetDevice(), m_ImageFences[Idx], nullptr);

		m_CurrentFrame = 0;
		m_CurrentImage = 0;

		Create(true);
	}

	bool VulkanSwapChain::AcquireNextImage()
	{
		auto Ctx = GraphicsContext::Get<VulkanContext>();
		vkWaitForFences(Ctx->GetDevice(), 1, &m_ImageFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);
		VkResult Result = vkAcquireNextImageKHR(Ctx->GetDevice(), m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &m_CurrentImage);

		if (Result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			Invalidate();
			return false;
		}

		vkResetFences(Ctx->GetDevice(), 1, &m_ImageFences[m_CurrentFrame]);
		return true;
	}
}