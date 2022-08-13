#pragma once

#include "Fusion/Core/Window.h"

//#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

namespace Fusion {

	struct QueueFamilyIndices
	{
		uint32_t GraphicsFamily = UINT32_MAX;
	};

	class VulkanContext
	{
	public:
		VulkanContext(const Window* window);
		~VulkanContext();

	private:
		bool CheckValidationLayers() const;
		QueueFamilyIndices GetQueueFamilies(const VkPhysicalDevice& device) const;

	private:
		VkInstance m_Instance = VK_NULL_HANDLE;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkDevice m_LogicalDevice = VK_NULL_HANDLE;
		VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
	};

}
