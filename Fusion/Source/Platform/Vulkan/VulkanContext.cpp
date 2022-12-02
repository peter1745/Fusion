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

	const std::vector<const char*> c_InstanceExtensions = { "VK_KHR_get_physical_device_properties2", "VK_EXT_debug_utils" };
	const std::vector<const char*> c_DeviceExtensions = { "VK_KHR_synchronization2" };

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

		// Find a suitable physical device
		FindSuitablePhysicalDevice();

		// Create Logical Device
		{
			float QueuePriority = 1.0f;
			VkDeviceQueueCreateInfo QueueInfo = {};
			QueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			QueueInfo.queueFamilyIndex = m_QueueFamily.QueueIndex;
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
			vkGetDeviceQueue(m_Device, m_QueueFamily.QueueIndex, 0, &m_QueueFamily.Queue);
		}

		VulkanLoader::LoadExtensionFunctions(m_Device);

		s_CurrentContext = this;
	}

	VulkanContext::~VulkanContext()
	{
	}

	void VulkanContext::Init(const Shared<SwapChain>& InSwapChain)
	{
		m_SwapChain = InSwapChain.As<VulkanSwapChain>();

		CommandAllocatorInfo AllocatorInfo = {};
		AllocatorInfo.ListType = ECommandListType::Direct;
		AllocatorInfo.InitialListCount = 1;

		for (uint32_t Idx = 0; Idx < m_SwapChain->GetFrameCount(); Idx++)
			m_CommandAllocators.push_back(Shared<VulkanCommandAllocator>::Create(m_Device, m_QueueFamily, AllocatorInfo));
	}

	void VulkanContext::FindSuitablePhysicalDevice()
	{
		uint32_t NumPhysicalDevices = 0;
		vkEnumeratePhysicalDevices(m_Instance, &NumPhysicalDevices, nullptr);
		std::vector<VkPhysicalDevice> AvailableDevices(NumPhysicalDevices);
		vkEnumeratePhysicalDevices(m_Instance, &NumPhysicalDevices, AvailableDevices.data());

		uint32_t MaxScore = 0;

		for (auto PhysicalDevice : AvailableDevices)
		{
			VkPhysicalDeviceProperties DeviceProperties;
			vkGetPhysicalDeviceProperties(PhysicalDevice, &DeviceProperties);

			VkPhysicalDeviceFeatures DeviceFeatures;
			vkGetPhysicalDeviceFeatures(PhysicalDevice, &DeviceFeatures);

			uint32_t Score = 0;

			// Give score based on GPU type
			switch (DeviceProperties.deviceType)
			{
			case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			{
				Score += 3;
				break;
			}
			case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			{
				Score += 4;
				break;
			}
			case VK_PHYSICAL_DEVICE_TYPE_CPU:
			{
				Score += 2;
				break;
			}
			case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			{
				Score += 1;
				break;
			}
			}

			// Queue Families
			{
				auto DeviceQueues = GetDeviceQueuesInfo(PhysicalDevice);

				for (const auto& QueueInfo : DeviceQueues)
				{
					if (QueueInfo.SupportsPresent && QueueInfo.SupportsGraphics)
						Score += 10;
				}
			}

			FUSION_CORE_INFO("GPU: {}, Score: {}", DeviceProperties.deviceName, Score);

			if (Score > MaxScore)
			{
				MaxScore = Score;
				m_PhysicalDevice = PhysicalDevice;
			}
		}

		FUSION_CORE_VERIFY(MaxScore != 0, "Found no suitable GPU");

		// Found suitable GPU, now determine Graphics + Present Queue
		auto DeviceQueues = GetDeviceQueuesInfo(m_PhysicalDevice);

		for (const auto& QueueInfo : DeviceQueues)
		{
			if (QueueInfo.SupportsPresent && QueueInfo.SupportsGraphics)
			{
				// Always prefer queues that have BOTH present and graphics capabilities
				m_QueueFamily = QueueInfo;
				break;
			}
		}

		FUSION_CORE_VERIFY(m_QueueFamily.IsValid(), "Failed to find suitable queue!");

		// Print GPU info
		VkPhysicalDeviceProperties DeviceProperties;
		vkGetPhysicalDeviceProperties(m_PhysicalDevice, &DeviceProperties);
		FUSION_CORE_INFO("Selected GPU: {} (ID={})", DeviceProperties.deviceName, DeviceProperties.deviceID);
	}

	void VulkanContext::ExecuteCommandLists(const std::vector<CommandList*>& InCommandLists)
	{
		std::vector<VkCommandBuffer> CommandBuffers;
		CommandBuffers.reserve(InCommandLists.size());
		for (auto* CommandList : InCommandLists)
			CommandBuffers.push_back(dynamic_cast<VulkanCommandList*>(CommandList)->GetBuffer());

		VkSubmitInfo SubmitInfo = {};
		SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		SubmitInfo.waitSemaphoreCount = 1;
		VkSemaphore WaitSemaphores[] = { m_SwapChain->GetImageAvailableSemaphore(m_SwapChain->GetFrameIndex()) };
		VkPipelineStageFlags WaitStageFlags[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		SubmitInfo.pWaitSemaphores = WaitSemaphores;
		SubmitInfo.pWaitDstStageMask = WaitStageFlags;
		SubmitInfo.commandBufferCount = InCommandLists.size();
		SubmitInfo.pCommandBuffers = CommandBuffers.data();

		VkSemaphore SignalSemaphores[] = { m_SwapChain->GetRenderFinishedSemaphore(m_SwapChain->GetFrameIndex()) };
		SubmitInfo.signalSemaphoreCount = 1;
		SubmitInfo.pSignalSemaphores = SignalSemaphores;

		FUSION_CORE_VERIFY(vkQueueSubmit(m_QueueFamily.Queue, 1, &SubmitInfo, m_SwapChain->GetImageFence(m_SwapChain->GetFrameIndex())) == VK_SUCCESS);
	}

	void VulkanContext::NextFrame()
	{
		if (!m_SwapChain->AcquireNextImage())
		{
			return;
		}

		uint32_t FrameIndex = m_SwapChain->GetFrameIndex();

		// Reset Command Pool
		m_CommandAllocators[FrameIndex]->Reset();
		m_CommandAllocators[FrameIndex]->GetCommandList(0)->BeginRecording();
	}

	void VulkanContext::WaitForGPU()
	{
		vkDeviceWaitIdle(m_Device);
	}

	std::vector<QueueFamilyInfo> VulkanContext::GetDeviceQueuesInfo(VkPhysicalDevice InDevice) const
	{
		uint32_t NumQueues = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(InDevice, &NumQueues, nullptr);
		std::vector<VkQueueFamilyProperties> QueueProperties(NumQueues);
		vkGetPhysicalDeviceQueueFamilyProperties(InDevice, &NumQueues, QueueProperties.data());

		std::vector<QueueFamilyInfo> Result(NumQueues);

		for (uint32_t QueueIdx = 0; QueueIdx < NumQueues; QueueIdx++)
		{
			auto& QueueInfo = Result[QueueIdx];
			QueueInfo.QueueIndex = QueueIdx;

			// Check for present support
			vkGetPhysicalDeviceSurfaceSupportKHR(InDevice, QueueIdx, m_Surface, reinterpret_cast<VkBool32*>(&QueueInfo.SupportsPresent));

			QueueInfo.SupportsGraphics = QueueProperties[QueueIdx].queueFlags & VK_QUEUE_GRAPHICS_BIT;
			QueueInfo.SupportCompute = QueueProperties[QueueIdx].queueFlags & VK_QUEUE_COMPUTE_BIT;
		}

		return Result;
	}

}
