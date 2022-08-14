#include "FusionPCH.h"
#include "VulkanSwapchain.h"

#include <glm/glm.hpp>

namespace Fusion {

	VulkanSwapchain::VulkanSwapchain(VkInstance InInstance, Shared<VulkanDevice> InDevice, VkSurfaceKHR InSurface)
		: m_Instance(InInstance), m_Device(InDevice), m_Surface(InSurface)
	{
	}

	VulkanSwapchain::~VulkanSwapchain()
	{
		VkDevice LogicalDevice = m_Device->GetLogicalDevice();
		vkDestroyFence(LogicalDevice, m_FrameInFlightFence, nullptr);
		vkDestroySemaphore(LogicalDevice, m_RenderFinishedSemaphore, nullptr);
		vkDestroySemaphore(LogicalDevice, m_ImageAvailableSemaphore, nullptr);

		for (uint32_t i = 0; i < m_Framebuffers.size(); i++)
			vkDestroyFramebuffer(LogicalDevice, m_Framebuffers[i], nullptr);
		m_Framebuffers.clear();

		vkDestroyRenderPass(LogicalDevice, m_RenderPass, nullptr);

		for (uint32_t i = 0; i < m_ImageViews.size(); i++)
			vkDestroyImageView(LogicalDevice, m_ImageViews[i], nullptr);
		m_ImageViews.clear();

		vkDestroySwapchainKHR(LogicalDevice, m_Swapchain, nullptr);
		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
	}

	void VulkanSwapchain::Create()
	{
		// Get image count
		uint32_t ImageCount = m_SurfaceCapabilities.minImageCount + 1;
		if (m_SurfaceCapabilities.maxImageCount > 0 && ImageCount > m_SurfaceCapabilities.maxImageCount)
			ImageCount = m_SurfaceCapabilities.maxImageCount;

		// Create the vulkan swapchain
		{
			VkSwapchainCreateInfoKHR SwapchainCreateInfo = {};
			SwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			SwapchainCreateInfo.surface = m_Surface;
			SwapchainCreateInfo.minImageCount = ImageCount;
			SwapchainCreateInfo.imageFormat = m_ImageFormat.format;
			SwapchainCreateInfo.imageColorSpace = m_ImageFormat.colorSpace;
			SwapchainCreateInfo.imageExtent = m_ImageExtent;
			SwapchainCreateInfo.imageArrayLayers = 1;
			SwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // VK_SHARING_MODE_CONCURRENT if present queue and graphics queue are different (assuming they're not right now)
			SwapchainCreateInfo.queueFamilyIndexCount = 0;
			SwapchainCreateInfo.pQueueFamilyIndices = nullptr;
			SwapchainCreateInfo.preTransform = m_SurfaceCapabilities.currentTransform;
			SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			SwapchainCreateInfo.presentMode = m_PresentMode;
			SwapchainCreateInfo.clipped = VK_TRUE;
			SwapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
			FUSION_CORE_VERIFY(vkCreateSwapchainKHR(m_Device->GetLogicalDevice(), &SwapchainCreateInfo, nullptr, &m_Swapchain) == VK_SUCCESS);
		}

		// Get swapchain images
		{
			FUSION_CORE_VERIFY(vkGetSwapchainImagesKHR(m_Device->GetLogicalDevice(), m_Swapchain, &ImageCount, nullptr) == VK_SUCCESS);
			m_Images.resize(ImageCount);
			m_ImageViews.resize(ImageCount);
			m_Framebuffers.resize(ImageCount);
			FUSION_CORE_VERIFY(vkGetSwapchainImagesKHR(m_Device->GetLogicalDevice(), m_Swapchain, &ImageCount, m_Images.data()) == VK_SUCCESS);
		}

		// Create Image Views
		{
			for (uint32_t i = 0; i < ImageCount; i++)
			{
				VkImageViewCreateInfo ImageViewCreateInfo = {};
				ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				ImageViewCreateInfo.image = m_Images[i];
				ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				ImageViewCreateInfo.format = m_ImageFormat.format;

				ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
				ImageViewCreateInfo.subresourceRange.levelCount = 1;
				ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
				ImageViewCreateInfo.subresourceRange.layerCount = 1;

				FUSION_CORE_VERIFY(vkCreateImageView(m_Device->GetLogicalDevice(), &ImageViewCreateInfo, nullptr, &m_ImageViews[i]) == VK_SUCCESS);
			}
		}

		// Create Default Render Pass
		{
			VkAttachmentDescription ColorAttachmentDescription = {};
			ColorAttachmentDescription.format = m_ImageFormat.format;
			ColorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
			ColorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			ColorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			ColorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			ColorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			VkAttachmentReference ColorAttachmentReference = {};
			ColorAttachmentReference.attachment = 0;
			ColorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			VkSubpassDescription SubpassDescription = {};
			SubpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			SubpassDescription.colorAttachmentCount = 1;
			SubpassDescription.pColorAttachments = &ColorAttachmentReference;

			VkSubpassDependency SubpassDependency = {};
			SubpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			SubpassDependency.dstSubpass = 0;
			SubpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			SubpassDependency.srcAccessMask = 0;
			SubpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			SubpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			VkRenderPassCreateInfo RenderPassCreateInfo = {};
			RenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			RenderPassCreateInfo.attachmentCount = 1;
			RenderPassCreateInfo.pAttachments = &ColorAttachmentDescription;
			RenderPassCreateInfo.subpassCount = 1;
			RenderPassCreateInfo.pSubpasses = &SubpassDescription;
			RenderPassCreateInfo.dependencyCount = 1;
			RenderPassCreateInfo.pDependencies = &SubpassDependency;

			vkCreateRenderPass(m_Device->GetLogicalDevice(), &RenderPassCreateInfo, nullptr, &m_RenderPass);
		}

		// Create Image Framebuffers
		{
			for (uint32_t i = 0; i < m_Framebuffers.size(); i++)
			{
				VkFramebufferCreateInfo FramebufferCreateInfo = {};
				FramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				FramebufferCreateInfo.renderPass = m_RenderPass;
				FramebufferCreateInfo.attachmentCount = 1;
				FramebufferCreateInfo.pAttachments = &m_ImageViews[i];
				FramebufferCreateInfo.width = m_ImageExtent.width;
				FramebufferCreateInfo.height = m_ImageExtent.height;
				FramebufferCreateInfo.layers = 1;

				vkCreateFramebuffer(m_Device->GetLogicalDevice(), &FramebufferCreateInfo, nullptr, &m_Framebuffers[i]);
			}
		}

		// Create Semaphores
		{
			VkSemaphoreCreateInfo ImageAvailableSemaphoreCreateInfo = {};
			ImageAvailableSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			vkCreateSemaphore(m_Device->GetLogicalDevice(), &ImageAvailableSemaphoreCreateInfo, nullptr, &m_ImageAvailableSemaphore);

			VkSemaphoreCreateInfo RenderFinishedSemaphoreCreateInfo = {};
			RenderFinishedSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			vkCreateSemaphore(m_Device->GetLogicalDevice(), &RenderFinishedSemaphoreCreateInfo, nullptr, &m_RenderFinishedSemaphore);
		}

		// Create Fence
		{
			VkFenceCreateInfo FrameInFlightCreateInfo = {};
			FrameInFlightCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			FrameInFlightCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			vkCreateFence(m_Device->GetLogicalDevice(), &FrameInFlightCreateInfo, nullptr, &m_FrameInFlightFence);
		}
	}

	void VulkanSwapchain::InitSurface(GLFWwindow* NativeWindow)
	{
		VkPhysicalDevice PhysicalDevice = m_Device->GetPhysicalDevice();

		FUSION_CORE_VERIFY(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, m_Surface, &m_SurfaceCapabilities) == VK_SUCCESS);

		// Get graphics queue index
		{
			const auto& QueueIndicies = m_Device->GetQueueIndicies();
			FUSION_CORE_VERIFY(QueueIndicies.IsValid());
			m_QueueIndex = QueueIndicies.GraphicsQueue;
		}

		// Find surface format and color format
		{
			uint32_t SurfaceFormatCount = 0;
			vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, m_Surface, &SurfaceFormatCount, nullptr);

			std::vector<VkSurfaceFormatKHR> SupportedFormats(SurfaceFormatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, m_Surface, &SurfaceFormatCount, SupportedFormats.data());

			bool FoundB8G8R8A8SRGBFormat = false;
			for (const auto& SurfaceFormat : SupportedFormats)
			{
				if (SurfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB && SurfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				{
					FoundB8G8R8A8SRGBFormat = true;
					m_ImageFormat = SurfaceFormat;
					break;
				}
			}

			if (!FoundB8G8R8A8SRGBFormat)
			{
				m_ImageFormat = SupportedFormats[0];
			}
		}

		// Find a suitable present mode
		{
			uint32_t PresentModesCount = 0;
			vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, m_Surface, &PresentModesCount, nullptr);

			std::vector<VkPresentModeKHR> SupportedModes(PresentModesCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, m_Surface, &PresentModesCount, SupportedModes.data());

			bool MailboxModeSupported = false;
			for (const auto& PresentMode : SupportedModes)
			{
				if (PresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					MailboxModeSupported = true;
					break;
				}
			}

			m_PresentMode = MailboxModeSupported ? VK_PRESENT_MODE_MAILBOX_KHR : VK_PRESENT_MODE_FIFO_KHR;
		}

		// Create Image Extent
		{
			if (m_SurfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			{
				m_ImageExtent = m_SurfaceCapabilities.currentExtent;
			}
			else
			{
				int32_t width = 0, height = 0;
				glfwGetFramebufferSize(NativeWindow, &width, &height);
				m_ImageExtent.width = glm::clamp(uint32_t(width), m_SurfaceCapabilities.minImageExtent.width, m_SurfaceCapabilities.maxImageExtent.width);
				m_ImageExtent.height = glm::clamp(uint32_t(height), m_SurfaceCapabilities.minImageExtent.height, m_SurfaceCapabilities.maxImageExtent.height);
			}
		}
	}

	void VulkanSwapchain::AquireNextFrame()
	{
		VkDevice LogicalDevice = m_Device->GetLogicalDevice();
		vkWaitForFences(LogicalDevice, 1, &m_FrameInFlightFence, VK_TRUE, UINT64_MAX);
		vkResetFences(LogicalDevice, 1, &m_FrameInFlightFence);
		vkAcquireNextImageKHR(LogicalDevice, m_Swapchain, UINT64_MAX, m_ImageAvailableSemaphore, VK_NULL_HANDLE, &m_CurrentImageIndex);
	}

	void VulkanSwapchain::SwapBuffers()
	{
		VkPresentInfoKHR PresentInfo = {};
		PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		PresentInfo.waitSemaphoreCount = 1;
		PresentInfo.pWaitSemaphores = &m_RenderFinishedSemaphore;
		PresentInfo.swapchainCount = 1;
		PresentInfo.pSwapchains = &m_Swapchain;
		PresentInfo.pImageIndices = &m_CurrentImageIndex;
		vkQueuePresentKHR(m_Device->GetGraphicsQueue(), &PresentInfo);
	}

}
