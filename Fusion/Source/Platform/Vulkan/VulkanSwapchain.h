#pragma once

#include "Fusion/Core/Window.h"

#include <vulkan/vulkan.h>

namespace Fusion {

	class VulkanSwapchain
	{
	public:
		// NOTE(Peter): Swapchain should be responsible for creating the surface
		VulkanSwapchain(VkInstance instance, VkDevice deviceInterface, VkPhysicalDevice device, VkSurfaceKHR surface, const Window* window);
		~VulkanSwapchain();

	private:
		VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
		VkSurfaceFormatKHR m_SurfaceFormat;
		VkPresentModeKHR m_PresentMode;
		VkSurfaceCapabilitiesKHR m_SurfaceCapabilities;

		std::vector<VkImage> m_Images;
		std::vector<VkImageView> m_ImageViews;
	};

}
