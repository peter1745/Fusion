#pragma once

#include <vulkan/vulkan.h>

namespace Fusion {

	struct VulkanDeviceQueueIndices
	{
		uint32_t GraphicsQueue = UINT32_MAX;
		uint32_t PresentQueue = UINT32_MAX;

		bool IsValid() const
		{
			return GraphicsQueue != UINT32_MAX && PresentQueue != UINT32_MAX;
		}
	};

	class VulkanDevice
	{
	public:
		VulkanDevice(VkInstance InInstance, VkSurfaceKHR InSurface);
		~VulkanDevice();

		VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
		VkDevice GetLogicalDevice() const { return m_LogicalDevice; }
		VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }

		const VulkanDeviceQueueIndices& GetQueueIndicies() const { return m_QueueIndices; }

	private:
		VulkanDeviceQueueIndices FindQueueIndicies(VkPhysicalDevice InDevice, VkSurfaceKHR InSurface) const;

	private:
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkDevice m_LogicalDevice = VK_NULL_HANDLE;
		VkQueue m_GraphicsQueue = VK_NULL_HANDLE;

		VulkanDeviceQueueIndices m_QueueIndices;
	};

}
