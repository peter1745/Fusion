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
		vkDestroySwapchainKHR(m_Device->GetLogicalDevice(), m_Swapchain, nullptr);
		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
	}

	void VulkanSwapchain::Create()
	{
		uint32_t ImageCount = m_SurfaceCapabilities.minImageCount + 1;

		if (m_SurfaceCapabilities.maxImageCount > 0 && ImageCount > m_SurfaceCapabilities.maxImageCount)
			ImageCount = m_SurfaceCapabilities.maxImageCount;

		VkSwapchainCreateInfoKHR SwapchainCreateInfo = {};
		SwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		SwapchainCreateInfo.surface = m_Surface;
		SwapchainCreateInfo.minImageCount = ImageCount;
		SwapchainCreateInfo.imageFormat = m_SurfaceFormat.format;
		SwapchainCreateInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
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

		// Get swapchain images
		FUSION_CORE_VERIFY(vkGetSwapchainImagesKHR(m_Device->GetLogicalDevice(), m_Swapchain, &ImageCount, nullptr) == VK_SUCCESS);
		m_Images.resize(ImageCount);
		FUSION_CORE_VERIFY(vkGetSwapchainImagesKHR(m_Device->GetLogicalDevice(), m_Swapchain, &ImageCount, m_Images.data()));
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
					m_SurfaceFormat = SurfaceFormat;
					break;
				}
			}

			if (!FoundB8G8R8A8SRGBFormat)
			{
				m_SurfaceFormat = SupportedFormats[0];
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

}
