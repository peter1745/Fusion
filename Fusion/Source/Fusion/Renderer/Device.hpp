#pragma once

#include "Fusion/Memory/Shared.hpp"

#include <vulkan/vulkan.h>

namespace Fusion {

	struct QueueInfo
	{
		uint32_t QueueFamily = UINT32_MAX;
		bool SupportsPresent = false;
		bool SupportsGraphics = false;
		bool SupportCompute = false;

		VkQueue Queue = VK_NULL_HANDLE;

		bool IsValid() const { return QueueFamily != UINT32_MAX; }
	};

	struct SurfaceProperties
	{
		std::vector<VkSurfaceFormatKHR> Formats;
		std::vector<VkPresentModeKHR> PresentModes;
		VkSurfaceCapabilitiesKHR Capabilities;
	};

	class Device : public SharedObject
	{
	public:
		Device(VkInstance InInstance, VkSurfaceKHR InSurface);

		void Wait() const;

		auto GetLogicalDevice() { return m_Device; }
		auto GetLogicalDevice() const { return m_Device; }

		auto GetPhysicalDevice() { return m_PhysicalDevice; }
		auto GetPhysicalDevice() const { return m_PhysicalDevice; }

		const auto& GetQueueInfo() const { return m_Queue; }

		void Release();

		SurfaceProperties GetSurfaceProperties(VkSurfaceKHR InSurface) const;

	private:
		void FindSuitablePhysicalDevice(VkInstance InInstance, VkSurfaceKHR InSurface);
		void CreateLogicalDevice();

		static std::vector<QueueInfo> GetQueues(VkPhysicalDevice InPhysicalDevice, VkSurfaceKHR InSurface);

	private:
		VkDevice m_Device = VK_NULL_HANDLE;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		QueueInfo m_Queue;
	};

}
