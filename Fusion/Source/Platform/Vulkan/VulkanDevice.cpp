#include "FusionPCH.h"
#include "VulkanDevice.h"

namespace Fusion {

	// TODO(Peter): Don't duplicate this...
#ifdef FUSION_DEBUG
	const bool c_EnableValidationLayers = true;
	const std::vector<const char*> c_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
#else
	const bool c_EnableValidationLayers = false;
#endif

	// TODO(Peter): Verify that physical device supports the extensions
	const std::vector<const char*> c_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	VulkanDevice::VulkanDevice(VkInstance InInstance, VkSurfaceKHR InSurface)
	{
		uint32_t PhysicalDeviceCount = 0;
		vkEnumeratePhysicalDevices(InInstance, &PhysicalDeviceCount, nullptr);
		FUSION_CORE_VERIFY(PhysicalDeviceCount > 0);

		std::vector<VkPhysicalDevice> AvailableDevices(PhysicalDeviceCount);
		vkEnumeratePhysicalDevices(InInstance, &PhysicalDeviceCount, AvailableDevices.data());

		for (auto Device : AvailableDevices)
		{
			VkPhysicalDeviceProperties DeviceProperties;
			vkGetPhysicalDeviceProperties(Device, &DeviceProperties);

			VkPhysicalDeviceFeatures DeviceFeatures;
			vkGetPhysicalDeviceFeatures(Device, &DeviceFeatures);

			// For now only support dedicated GPUs
			if (DeviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				continue;

			VulkanDeviceQueueIndices QueueIndices = FindQueueIndicies(Device, InSurface);
			if (!QueueIndices.IsValid())
				continue;

			m_PhysicalDevice = Device;
			m_QueueIndices = QueueIndices;
			break;
		}

		FUSION_CORE_VERIFY(m_PhysicalDevice != VK_NULL_HANDLE);
		FUSION_CORE_VERIFY(m_QueueIndices.GraphicsQueue == m_QueueIndices.PresentQueue);

		// Setup Logical Device
		float QueuePriority = 1.0f;
		VkDeviceQueueCreateInfo GraphicsQueueCreateInfo = {};
		GraphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		GraphicsQueueCreateInfo.queueFamilyIndex = m_QueueIndices.GraphicsQueue;
		GraphicsQueueCreateInfo.queueCount = 1;
		GraphicsQueueCreateInfo.pQueuePriorities = &QueuePriority;

		VkPhysicalDeviceVulkan13Features Vulkan13DeviceFeatures = {};
		Vulkan13DeviceFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
		Vulkan13DeviceFeatures.dynamicRendering = VK_TRUE;
		Vulkan13DeviceFeatures.synchronization2 = VK_TRUE;

		VkPhysicalDeviceFeatures2 PhysicalDeviceFeatures2 = {};
		PhysicalDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		PhysicalDeviceFeatures2.pNext = &Vulkan13DeviceFeatures;

		VkDeviceCreateInfo DeviceCreateInfo = {};
		DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		DeviceCreateInfo.pQueueCreateInfos = &GraphicsQueueCreateInfo;
		DeviceCreateInfo.queueCreateInfoCount = 1;
		DeviceCreateInfo.enabledExtensionCount = uint32_t(c_DeviceExtensions.size());
		DeviceCreateInfo.ppEnabledExtensionNames = c_DeviceExtensions.data();
		DeviceCreateInfo.pNext = &PhysicalDeviceFeatures2;

		if (c_EnableValidationLayers)
		{
			DeviceCreateInfo.enabledLayerCount = uint32_t(c_ValidationLayers.size());
			DeviceCreateInfo.ppEnabledLayerNames = c_ValidationLayers.data();
		}

		FUSION_CORE_VERIFY(vkCreateDevice(m_PhysicalDevice, &DeviceCreateInfo, nullptr, &m_LogicalDevice) == VK_SUCCESS);

		// Get the graphics queue
		vkGetDeviceQueue(m_LogicalDevice, m_QueueIndices.GraphicsQueue, 0, &m_GraphicsQueue);
	}

	VulkanDevice::~VulkanDevice()
	{
		vkDestroyDevice(m_LogicalDevice, nullptr);
	}

	VulkanDeviceQueueIndices VulkanDevice::FindQueueIndicies(VkPhysicalDevice InDevice, VkSurfaceKHR InSurface) const
	{
		VulkanDeviceQueueIndices QueueIndicies;

		uint32_t QueueCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(InDevice, &QueueCount, nullptr);

		std::vector<VkQueueFamilyProperties> QueueFamilyProperties(QueueCount);
		vkGetPhysicalDeviceQueueFamilyProperties(InDevice, &QueueCount, QueueFamilyProperties.data());

		std::vector<VkBool32> SupportsPresenting(QueueCount);
		for (uint32_t i = 0; i < QueueCount; i++)
			vkGetPhysicalDeviceSurfaceSupportKHR(InDevice, i, InSurface, &SupportsPresenting[i]);

		for (uint32_t i = 0; i < QueueCount; i++)
		{
			if ((QueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)
				continue;

			if (SupportsPresenting[i] == VK_TRUE)
			{
				QueueIndicies.GraphicsQueue = i;
				QueueIndicies.PresentQueue = i;
				break;
			}
		}

		return QueueIndicies;
	}

}
