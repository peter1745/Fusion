#pragma once

#include "VulkanDevice.h"
#include <GLFW/glfw3.h>

namespace Fusion {

	class VulkanSwapchain
	{
	public:
		// NOTE(Peter): Swapchain should be responsible for creating the surface
		VulkanSwapchain(VkInstance InInstance, Shared<VulkanDevice> InDevice, VkSurfaceKHR InSurface);
		~VulkanSwapchain();

		void Create();
		void InitSurface(GLFWwindow* NativeWindow);

	private:
		VkInstance m_Instance = VK_NULL_HANDLE;
		Shared<VulkanDevice> m_Device;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
		VkExtent2D m_ImageExtent = {};
		uint32_t m_QueueIndex = UINT32_MAX;

		VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
		VkSurfaceFormatKHR m_SurfaceFormat;
		VkPresentModeKHR m_PresentMode;
		VkSurfaceCapabilitiesKHR m_SurfaceCapabilities;

		std::vector<VkImage> m_Images;
		std::vector<VkImageView> m_ImageViews;
	};

}
