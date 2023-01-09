#pragma once

#include "Device.hpp"
#include "CommandPool.hpp"
#include "VulkanAllocator.hpp"

namespace Fusion {

	class GraphicsContext : public SharedObject
	{
	public:
		GraphicsContext();
		~GraphicsContext();

		Shared<Device> GetDevice() const { return m_Device; }

		void Release();

		auto GetTemporaryCommandAllocator() { return m_TemporaryCommandAllocator; }
		auto GetTemporaryCommandAllocator() const { return m_TemporaryCommandAllocator; }

		auto* GetTemporaryCommandList() { return m_TemporaryCommandAllocator->GetCommandBuffer(0); }
		auto* GetTemporaryCommandList() const { return m_TemporaryCommandAllocator->GetCommandBuffer(0); }

		auto GetInstance() { return m_Instance; }
		auto GetInstance() const { return m_Instance; }

		auto GetSurface() { return m_Surface; }
		auto GetSurface() const { return m_Surface; }

		auto* GetAllocator() const { return m_Allocator.get(); }

	public:
		static GraphicsContext* Get();

	private:
		VkInstance m_Instance = VK_NULL_HANDLE;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

		Shared<Device> m_Device = nullptr;

		Shared<CommandPool> m_TemporaryCommandAllocator = nullptr;

		Unique<VulkanAllocator> m_Allocator = nullptr;
	};

}
