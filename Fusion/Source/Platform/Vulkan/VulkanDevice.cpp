#include "VulkanDevice.hpp"
#include "VulkanFunctions.hpp"
#include "VulkanCommandList.hpp"

namespace Fusion {

	static const std::vector<const char*> c_DeviceExtensions = { "VK_KHR_synchronization2" };

	VulkanDevice::VulkanDevice(VkInstance InInstance, VkSurfaceKHR InSurface)
	{
		FindSuitablePhysicalDevice(InInstance, InSurface);
		CreateLogicalDevice();

		VulkanLoader::LoadExtensionFunctions(m_Device, InInstance);
	}

	void VulkanDevice::Wait() const
	{
		vkDeviceWaitIdle(m_Device);
	}

	void VulkanDevice::Release()
	{
		vkDeviceWaitIdle(m_Device);
		vkDestroyDevice(m_Device, nullptr);

		m_Device = VK_NULL_HANDLE;
		m_PhysicalDevice = VK_NULL_HANDLE;
	}

	void VulkanDevice::FindSuitablePhysicalDevice(VkInstance InInstance, VkSurfaceKHR InSurface)
	{
		uint32_t NumPhysicalDevices = 0;
		vkEnumeratePhysicalDevices(InInstance, &NumPhysicalDevices, nullptr);
		std::vector<VkPhysicalDevice> AvailableDevices(NumPhysicalDevices);
		vkEnumeratePhysicalDevices(InInstance, &NumPhysicalDevices, AvailableDevices.data());

		uint32_t MaxScore = 0;

		for (auto PhysicalDevice : AvailableDevices)
		{
			VkPhysicalDeviceProperties DeviceProperties;
			vkGetPhysicalDeviceProperties(PhysicalDevice, &DeviceProperties);

			FUSION_CORE_INFO("GPU: {} ({})", DeviceProperties.deviceName, DeviceProperties.deviceID);
			FUSION_CORE_INFO("\tDriver Version: {}", DeviceProperties.driverVersion);

			VkPhysicalDeviceFeatures DeviceFeatures;
			vkGetPhysicalDeviceFeatures(PhysicalDevice, &DeviceFeatures);

			uint32_t Score = 0;

			// Give score based on GPU type
			switch (DeviceProperties.deviceType)
			{
			case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			{
				FUSION_CORE_INFO("\tType: Integrated");
				Score += 3;
				break;
			}
			case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			{
				FUSION_CORE_INFO("\tType: Discrete");
				Score += 4;
				break;
			}
			case VK_PHYSICAL_DEVICE_TYPE_CPU:
			{
				FUSION_CORE_INFO("\tType: CPU");
				Score += 2;
				break;
			}
			case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			{
				FUSION_CORE_INFO("\tType: Virtual");
				Score += 1;
				break;
			}
			}

			// Queue Families
			{
				auto DeviceQueues = GetQueues(PhysicalDevice, InSurface);

				for (const auto& QueueInfo : DeviceQueues)
				{
					if (QueueInfo.SupportsPresent && QueueInfo.SupportsGraphics)
						Score += 10;
				}
			}

			FUSION_CORE_INFO("\tScore: {}", DeviceProperties.deviceName, Score);

			if (Score > MaxScore)
			{
				MaxScore = Score;
				m_PhysicalDevice = PhysicalDevice;
			}
		}

		FUSION_CORE_VERIFY(MaxScore != 0, "Found no suitable GPU");

		// Found suitable GPU, now determine Graphics + Present Queue
		auto DeviceQueues = GetQueues(m_PhysicalDevice, InSurface);

		for (const auto& QueueInfo : DeviceQueues)
		{
			if (QueueInfo.SupportsPresent && QueueInfo.SupportsGraphics)
			{
				// Always prefer queues that have BOTH present and graphics capabilities
				m_Queue = QueueInfo;
				break;
			}
		}

		FUSION_CORE_VERIFY(m_Queue.IsValid(), "Failed to find suitable queue!");

		// Print GPU info
		VkPhysicalDeviceProperties DeviceProperties;
		vkGetPhysicalDeviceProperties(m_PhysicalDevice, &DeviceProperties);
		FUSION_CORE_INFO("Selected GPU: {} (ID={})", DeviceProperties.deviceName, DeviceProperties.deviceID);
	}

	void VulkanDevice::CreateLogicalDevice()
	{
		float QueuePriority = 1.0f;
		VkDeviceQueueCreateInfo QueueInfo = {};
		QueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		QueueInfo.queueFamilyIndex = m_Queue.QueueFamily;
		QueueInfo.queueCount = 1;
		QueueInfo.pQueuePriorities = &QueuePriority;

		VkPhysicalDeviceSynchronization2Features Synchronization2 = {};
		Synchronization2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
		Synchronization2.synchronization2 = VK_TRUE;

		VkPhysicalDeviceFeatures2 DeviceFeatures2 = {};
		DeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		DeviceFeatures2.pNext = &Synchronization2;

		VkDeviceCreateInfo DeviceInfo = {};
		DeviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		DeviceInfo.pQueueCreateInfos = &QueueInfo;
		DeviceInfo.queueCreateInfoCount = 1;
		DeviceInfo.pNext = &DeviceFeatures2;

		std::vector<const char*> RequiredExtensions(c_DeviceExtensions);
		RequiredExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		DeviceInfo.ppEnabledExtensionNames = RequiredExtensions.data();
		DeviceInfo.enabledExtensionCount = RequiredExtensions.size();

		FUSION_CORE_VERIFY(vkCreateDevice(m_PhysicalDevice, &DeviceInfo, nullptr, &m_Device) == VK_SUCCESS);
		vkGetDeviceQueue(m_Device, m_Queue.QueueFamily, 0, &m_Queue.Queue);
	}

	std::vector<QueueInfo> VulkanDevice::GetQueues(VkPhysicalDevice InPhysicalDevice, VkSurfaceKHR InSurface) const
	{
		uint32_t NumQueues = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(InPhysicalDevice, &NumQueues, nullptr);
		std::vector<VkQueueFamilyProperties> QueueProperties(NumQueues);
		vkGetPhysicalDeviceQueueFamilyProperties(InPhysicalDevice, &NumQueues, QueueProperties.data());

		std::vector<QueueInfo> Result(NumQueues);

		for (uint32_t QueueIdx = 0; QueueIdx < NumQueues; QueueIdx++)
		{
			auto& QueueInfo = Result[QueueIdx];
			QueueInfo.QueueFamily = QueueIdx;

			// Check for present support
			vkGetPhysicalDeviceSurfaceSupportKHR(InPhysicalDevice, QueueIdx, InSurface, reinterpret_cast<VkBool32*>(&QueueInfo.SupportsPresent));

			QueueInfo.SupportsGraphics = QueueProperties[QueueIdx].queueFlags & VK_QUEUE_GRAPHICS_BIT;
			QueueInfo.SupportCompute = QueueProperties[QueueIdx].queueFlags & VK_QUEUE_COMPUTE_BIT;
		}

		return Result;
	}

}