#include "FusionPCH.hpp"
#include "VulkanContext.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanFunctions.hpp"

#include "Fusion/Core/Application.hpp"

#include <queue>

#include <GLFW/glfw3.h>

namespace Fusion {

#ifdef FUSION_DEBUG
	static constexpr bool s_EnableValidation = true;
	static const std::vector<const char*> s_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
#else
	static constexpr bool s_EnableValidation = false;
#endif

	static const std::vector<const char*> c_InstanceExtensions = { "VK_KHR_get_physical_device_properties2", "VK_EXT_debug_utils" };

	VulkanContext::VulkanContext()
	{
		VkApplicationInfo AppInfo = {};
		AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		AppInfo.pApplicationName = "Fusion Vulkan App";
		AppInfo.pEngineName = "Fusion";
		AppInfo.apiVersion = VK_API_VERSION_1_2;

		// Create Vulkan Instance
		{
			VkInstanceCreateInfo InstanceInfo = {};
			InstanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			InstanceInfo.pApplicationInfo = &AppInfo;

			// Instance extensions
			uint32_t NumGLFWExtensions = 0;
			const char** GLFWExtensions = glfwGetRequiredInstanceExtensions(&NumGLFWExtensions);
			std::vector<const char*> InstanceExtensions(NumGLFWExtensions);
			for (uint32_t Idx = 0; Idx < NumGLFWExtensions; Idx++)
				InstanceExtensions[Idx] = GLFWExtensions[Idx];

			for (auto Ext : c_InstanceExtensions)
				InstanceExtensions.push_back(Ext);

			InstanceInfo.enabledExtensionCount = InstanceExtensions.size();
			InstanceInfo.ppEnabledExtensionNames = InstanceExtensions.data();

			if constexpr (s_EnableValidation)
			{
				InstanceInfo.enabledLayerCount = s_ValidationLayers.size();
				InstanceInfo.ppEnabledLayerNames = s_ValidationLayers.data();
			}

			FUSION_CORE_VERIFY(vkCreateInstance(&InstanceInfo, nullptr, &m_Instance) == VK_SUCCESS);
		}

		// Create Window Surface via GLFW
		{
			GLFWwindow* NativeWindow = static_cast<GLFWwindow*>(Application::Get().GetWindow()->GetWindowHandle());
			glfwCreateWindowSurface(m_Instance, NativeWindow, nullptr, &m_Surface);
		}

		m_Device = Shared<VulkanDevice>::Create(m_Instance, m_Surface);

		CommandAllocatorInfo AllocatorInfo = {};
		AllocatorInfo.InitialListCount = 1;
		m_TemporaryCommandAllocator = Shared<VulkanCommandAllocator>::Create(m_Device, AllocatorInfo);

		s_CurrentContext = this;
	}

	VulkanContext::~VulkanContext()
	{
		evkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);

		vkDestroyInstance(m_Instance, nullptr);
	}
}
