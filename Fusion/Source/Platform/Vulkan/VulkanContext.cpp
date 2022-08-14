#include "FusionPCH.h"
#include "VulkanContext.h"

//#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
//#include <GLFW/glfw3native.h>

#include <glm/glm.hpp>

#include <iostream>

namespace Fusion {

#ifdef FUSION_DEBUG
	const bool c_EnableValidationLayers = true;
	const std::vector<const char*> c_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
#else
	const bool c_EnableValidationLayers = false;
#endif

	const std::vector<const char*> c_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	VulkanContext::VulkanContext(const Window* window)
	{
		FUSION_CORE_VERIFY(c_EnableValidationLayers && CheckValidationLayers());

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Fusion Vulkan App";
		appInfo.pEngineName = "Fusion";
		appInfo.apiVersion = VK_API_VERSION_1_2;

		VkInstanceCreateInfo instanceCreateInfo{};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo = &appInfo;
		
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		instanceCreateInfo.enabledExtensionCount = glfwExtensionCount;
		instanceCreateInfo.ppEnabledExtensionNames = glfwExtensions;

		if (c_EnableValidationLayers)
		{
			instanceCreateInfo.enabledLayerCount = uint32_t(c_ValidationLayers.size());
			instanceCreateInfo.ppEnabledLayerNames = c_ValidationLayers.data();
		}

		FUSION_CORE_VERIFY(vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance) == VK_SUCCESS);

		GLFWwindow* nativeWindow = static_cast<GLFWwindow*>(window->GetNativeWindow());

#ifdef FUSION_PLATFORM_WINDOWS
		// Setup Surface
		VkResult surfaceResult = glfwCreateWindowSurface(m_Instance, nativeWindow, nullptr, &m_Surface);
		if (surfaceResult != VK_SUCCESS)
		{
			__debugbreak();
			return;
		}
#else
	#error Platform not supported!
#endif

		// Setup Physical Device
		uint32_t physicalDeviceCount = 0;
		vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, nullptr);
		FUSION_CORE_VERIFY(physicalDeviceCount != 0);

		std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
		vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, physicalDevices.data());

		for (const auto& device : physicalDevices)
		{
			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceProperties(device, &deviceProperties);

			VkPhysicalDeviceFeatures deviceFeatures;
			vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

			if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				continue;

			QueueFamilyIndices queueFamilyIndices = GetQueueFamilies(device);

			if (queueFamilyIndices.GraphicsFamily == UINT32_MAX)
				continue;

			m_PhysicalDevice = device;
			break;
		}

		// Setup Logical Device
		QueueFamilyIndices queueFamilyIndices = GetQueueFamilies(m_PhysicalDevice);

		float queuePriority = 1.0f;
		VkDeviceQueueCreateInfo graphicsQueueCreateInfo{};
		graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		graphicsQueueCreateInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily;
		graphicsQueueCreateInfo.queueCount = 1;
		graphicsQueueCreateInfo.pQueuePriorities = &queuePriority;

		VkPhysicalDeviceFeatures deviceFeatures{};

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pQueueCreateInfos = &graphicsQueueCreateInfo;
		deviceCreateInfo.queueCreateInfoCount = 1;
		deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
		deviceCreateInfo.enabledExtensionCount = uint32_t(c_DeviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = c_DeviceExtensions.data();

		if (c_EnableValidationLayers)
		{
			deviceCreateInfo.enabledLayerCount = uint32_t(c_ValidationLayers.size());
			deviceCreateInfo.ppEnabledLayerNames = c_ValidationLayers.data();
		}

		FUSION_CORE_VERIFY(vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_LogicalDevice) == VK_SUCCESS);

		vkGetDeviceQueue(m_LogicalDevice, queueFamilyIndices.GraphicsFamily, 0, &m_GraphicsQueue);

		m_Swapchain = std::make_shared<VulkanSwapchain>(m_Instance, m_LogicalDevice, m_PhysicalDevice, m_Surface, window);
	}

	VulkanContext::~VulkanContext()
	{
		vkDestroyDevice(m_LogicalDevice, nullptr);
		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		vkDestroyInstance(m_Instance, nullptr);
	}

	bool VulkanContext::CheckValidationLayers() const
	{
		uint32_t supportedLayerCount = 0;
		vkEnumerateInstanceLayerProperties(&supportedLayerCount, nullptr);

		std::vector<VkLayerProperties> supportedLayers(supportedLayerCount);
		vkEnumerateInstanceLayerProperties(&supportedLayerCount, supportedLayers.data());

		for (const char* requestedLayerName : c_ValidationLayers)
		{
			bool foundLayer = false;

			for (const auto& layer : supportedLayers)
			{
				if (strcmp(requestedLayerName, layer.layerName) == 0)
				{
					foundLayer = true;
					break;
				}
			}

			if (!foundLayer)
				return false;
		}

		return true;
	}

	QueueFamilyIndices VulkanContext::GetQueueFamilies(VkPhysicalDevice device) const
	{
		QueueFamilyIndices queueFamilyIndices;
		
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		for (uint32_t i = 0; i < queueFamilyCount; i++)
		{
			const auto& queueFamily = queueFamilies[i];

			VkBool32 surfaceSupported = VK_FALSE;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &surfaceSupported);

			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT && surfaceSupported)
				queueFamilyIndices.GraphicsFamily = i;
		}

		return queueFamilyIndices;
	}
}
