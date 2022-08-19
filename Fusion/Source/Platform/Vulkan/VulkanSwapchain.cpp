#include "FusionPCH.h"
#include "VulkanSwapchain.h"
#include "Fusion/Core/Application.h"

#include <glm/glm.hpp>

namespace Fusion {

	VulkanSwapchain::VulkanSwapchain(VkInstance InInstance, Shared<VulkanDevice> InDevice, VkSurfaceKHR InSurface)
		: m_Instance(InInstance), m_Device(InDevice), m_Surface(InSurface)
	{
	}

	VulkanSwapchain::~VulkanSwapchain()
	{
		VkDevice LogicalDevice = m_Device->GetLogicalDevice();

		for (uint32_t i = 0; i < m_MaxFramesInFlight; i++)
		{
			vkDestroyFence(LogicalDevice, m_FrameInFlightFences[i], nullptr);
			vkDestroySemaphore(LogicalDevice, m_RenderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(LogicalDevice, m_ImageAvailableSemaphores[i], nullptr);
		}

		for (uint32_t i = 0; i < m_ImageViews.size(); i++)
			vkDestroyImageView(LogicalDevice, m_ImageViews[i], nullptr);

		vkDestroySwapchainKHR(LogicalDevice, m_Swapchain, nullptr);
		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
	}

	void VulkanSwapchain::Create(bool InWasInvalidated)
	{
		// Get image count
		uint32_t ImageCount = m_SurfaceCapabilities.minImageCount + 1;
		if (m_SurfaceCapabilities.maxImageCount > 0 && ImageCount > m_SurfaceCapabilities.maxImageCount)
			ImageCount = m_SurfaceCapabilities.maxImageCount;

		m_MaxFramesInFlight = ImageCount;

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

		// Create Semaphores & Fences
		if (!InWasInvalidated)
		{
			m_ImageAvailableSemaphores.resize(m_MaxFramesInFlight);
			m_RenderFinishedSemaphores.resize(m_MaxFramesInFlight);
			m_FrameInFlightFences.resize(m_MaxFramesInFlight);

			VkSemaphoreCreateInfo SemaphoreCreateInfo = {};
			SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			VkFenceCreateInfo FrameInFlightCreateInfo = {};
			FrameInFlightCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			FrameInFlightCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			for (uint32_t i = 0; i < m_MaxFramesInFlight; i++)
			{
				vkCreateSemaphore(m_Device->GetLogicalDevice(), &SemaphoreCreateInfo, nullptr, &m_ImageAvailableSemaphores[i]);
				vkCreateSemaphore(m_Device->GetLogicalDevice(), &SemaphoreCreateInfo, nullptr, &m_RenderFinishedSemaphores[i]);
				vkCreateFence(m_Device->GetLogicalDevice(), &FrameInFlightCreateInfo, nullptr, &m_FrameInFlightFences[i]);
			}
		}
	}

	void VulkanSwapchain::InitSurface(GLFWwindow* NativeWindow)
	{
		VkPhysicalDevice PhysicalDevice = m_Device->GetPhysicalDevice();

		FUSION_CORE_VERIFY(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, m_Surface, &m_SurfaceCapabilities) == VK_SUCCESS);

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

	void VulkanSwapchain::Invalidate()
	{
		VkDevice LogicalDevice = m_Device->GetLogicalDevice();

		// Ensure we aren't accessing any resources
		vkDeviceWaitIdle(LogicalDevice);

		// Destroy image views and swapchain
		for (uint32_t i = 0; i < m_ImageViews.size(); i++)
			vkDestroyImageView(LogicalDevice, m_ImageViews[i], nullptr);
		vkDestroySwapchainKHR(LogicalDevice, m_Swapchain, nullptr);

		GLFWwindow* NativeWindow = static_cast<GLFWwindow*>(Application::Get().GetWindow()->GetNativeWindow());
		InitSurface(NativeWindow);
		Create(true);
	}

	bool VulkanSwapchain::AquireNextFrame()
	{
		VkDevice LogicalDevice = m_Device->GetLogicalDevice();
		vkWaitForFences(LogicalDevice, 1, &m_FrameInFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);
		VkResult result = vkAcquireNextImageKHR(LogicalDevice, m_Swapchain, UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &m_CurrentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			Invalidate();
			return false;
		}

		FUSION_CORE_VERIFY(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "Unable to aquire the next swapchain image!");

		vkResetFences(LogicalDevice, 1, &m_FrameInFlightFences[m_CurrentFrame]);
		return true;
	}

	void VulkanSwapchain::SwapBuffers()
	{
		VkPresentInfoKHR PresentInfo = {};
		PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		PresentInfo.waitSemaphoreCount = 1;
		PresentInfo.pWaitSemaphores = &m_RenderFinishedSemaphores[m_CurrentFrame];
		PresentInfo.swapchainCount = 1;
		PresentInfo.pSwapchains = &m_Swapchain;
		PresentInfo.pImageIndices = &m_CurrentImageIndex;
		VkResult result = vkQueuePresentKHR(m_Device->GetGraphicsQueue(), &PresentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			Invalidate();
		}
		else if (result != VK_SUCCESS)
		{
			FUSION_CORE_VERIFY(false, "Error when trying to present current frame!");
		}

		m_CurrentFrame = (m_CurrentFrame + 1) % m_MaxFramesInFlight;
	}

}
