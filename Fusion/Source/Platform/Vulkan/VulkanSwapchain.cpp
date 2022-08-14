#include "FusionPCH.h"
#include "VulkanSwapchain.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace Fusion {

	VulkanSwapchain::VulkanSwapchain(VkInstance instance, VkDevice deviceInterface, VkPhysicalDevice device, VkSurfaceKHR surface, const Window* window)
	{
		// Find a suitable surface format and color space
		{
			uint32_t surfaceFormatCount = 0;
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &surfaceFormatCount, nullptr);

			std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &surfaceFormatCount, surfaceFormats.data());

			bool srgbSupported = false;
			for (const auto& surfaceFormat : surfaceFormats)
			{
				if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB && surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				{
					srgbSupported = true;
					m_SurfaceFormat = surfaceFormat;
					break;
				}
			}

			if (!srgbSupported)
				m_SurfaceFormat = surfaceFormats[0];
		}

		// Pick a presentation mode
		{
			uint32_t presentModesCount = 0;
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModesCount, nullptr);

			std::vector<VkPresentModeKHR> presentModes(presentModesCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModesCount, presentModes.data());

			bool mailboxModeSupported = false;
			for (const auto& presentMode : presentModes)
			{
				if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					mailboxModeSupported = true;
					break;
				}
			}

			m_PresentMode = mailboxModeSupported ? VK_PRESENT_MODE_MAILBOX_KHR : VK_PRESENT_MODE_FIFO_KHR;
		}

		VkExtent2D swapchainExtent = {};

		// Surface Capabilities
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &m_SurfaceCapabilities);

		// Image Extent
		{
			if (m_SurfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			{
				swapchainExtent = m_SurfaceCapabilities.currentExtent;
			}
			else
			{
				GLFWwindow* nativeWindow = static_cast<GLFWwindow*>(window->GetNativeWindow());
				int32_t width = 0, height = 0;
				glfwGetFramebufferSize(nativeWindow, &width, &height);

				swapchainExtent.width = glm::clamp(uint32_t(width), m_SurfaceCapabilities.minImageExtent.width, m_SurfaceCapabilities.maxImageExtent.width);
				swapchainExtent.height = glm::clamp(uint32_t(height), m_SurfaceCapabilities.minImageExtent.height, m_SurfaceCapabilities.maxImageExtent.height);
			}
		}

		uint32_t imageCount = m_SurfaceCapabilities.minImageCount + 1;

		if (m_SurfaceCapabilities.maxImageCount > 0 && imageCount > m_SurfaceCapabilities.maxImageCount)
			imageCount = m_SurfaceCapabilities.maxImageCount;

		VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
		swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfo.surface = surface;
		swapchainCreateInfo.minImageCount = imageCount;
		swapchainCreateInfo.imageFormat = m_SurfaceFormat.format;
		swapchainCreateInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
		swapchainCreateInfo.imageExtent = swapchainExtent;
		swapchainCreateInfo.imageArrayLayers = 1;
		swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // VK_SHARING_MODE_CONCURRENT if present queue and graphics queue are different (assuming they're not right now)
		swapchainCreateInfo.queueFamilyIndexCount = 0;
		swapchainCreateInfo.pQueueFamilyIndices = nullptr;
		swapchainCreateInfo.preTransform = m_SurfaceCapabilities.currentTransform;
		swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainCreateInfo.presentMode = m_PresentMode;
		swapchainCreateInfo.clipped = VK_TRUE;
		swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

		FUSION_CORE_VERIFY(vkCreateSwapchainKHR(deviceInterface, &swapchainCreateInfo, nullptr, &m_Swapchain) == VK_SUCCESS);
	}

	VulkanSwapchain::~VulkanSwapchain()
	{
		//vkDestroySwapchainKHR()
	}

}
