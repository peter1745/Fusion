#pragma once

#include "Fusion/Core/Window.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"

//#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

namespace Fusion {

	struct SwapchainCapabilities
	{
		VkSurfaceCapabilitiesKHR SurfaceCapabilities;
		std::vector<VkSurfaceFormatKHR> Formats;
		std::vector<VkPresentModeKHR> PresentModes;
	};

	class VulkanContext
	{
	public:
		VulkanContext(const Window* InWindow);
		~VulkanContext();

	private:
		bool CheckValidationLayers() const;

	private:
		VkInstance m_Instance = VK_NULL_HANDLE;

		Shared<VulkanDevice> m_Device = nullptr;
		Shared<VulkanSwapchain> m_Swapchain = nullptr;
	};

}
