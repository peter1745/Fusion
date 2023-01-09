#pragma once

#include "Fusion/Renderer/GraphicsContext.hpp"
#include "VulkanDevice.hpp"
#include "VulkanCommandAllocator.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanAllocator.hpp"

namespace Fusion {

	class VulkanContext : public GraphicsContext
	{
	public:
		VulkanContext();
		~VulkanContext();

		Shared<Device> GetDevice() const override { return m_Device; }

		virtual Shared<DescriptorHeap> GetDescriptorHeap(EDescriptorHeapType InType) const override
		{
			return nullptr;
		}

		void Release() override;

		auto GetTemporaryCommandAllocator() { return m_TemporaryCommandAllocator; }
		auto GetTemporaryCommandAllocator() const { return m_TemporaryCommandAllocator; }

		auto* GetTemporaryCommandList() { return m_TemporaryCommandAllocator->GetCommandList(0); }
		auto* GetTemporaryCommandList() const { return m_TemporaryCommandAllocator->GetCommandList(0); }

		auto GetInstance() { return m_Instance; }
		auto GetInstance() const { return m_Instance; }

		auto GetSurface() { return m_Surface; }
		auto GetSurface() const { return m_Surface; }

		auto* GetAllocator() const { return m_Allocator.get(); }

	private:
		VkInstance m_Instance = VK_NULL_HANDLE;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

		Shared<VulkanDevice> m_Device = nullptr;

		Shared<VulkanCommandAllocator> m_TemporaryCommandAllocator = nullptr;

		Unique<VulkanAllocator> m_Allocator = nullptr;
	};

}
