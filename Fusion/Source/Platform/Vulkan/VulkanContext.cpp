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

	VulkanContext::VulkanContext(const Window* InWindow)
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

		GLFWwindow* NativeWindow = static_cast<GLFWwindow*>(InWindow->GetNativeWindow());
		VkSurfaceKHR Surface = VK_NULL_HANDLE;
		FUSION_CORE_VERIFY(glfwCreateWindowSurface(m_Instance, NativeWindow, nullptr, &Surface) == VK_SUCCESS);

		m_Device = MakeShared<VulkanDevice>(m_Instance, Surface);
		m_Swapchain = MakeShared<VulkanSwapchain>(m_Instance, m_Device, Surface);
		m_Swapchain->InitSurface(NativeWindow);
		m_Swapchain->Create();
	}

	VulkanContext::~VulkanContext()
	{
		m_Swapchain.reset();
		m_Device.reset();

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

}
