#pragma once

#include "Fusion/Renderer/GraphicsContext.hpp"
#include "VulkanQueueFamily.hpp"
#include "VulkanCommandAllocator.hpp"
#include "VulkanSwapChain.hpp"

namespace Fusion {

	class VulkanContext : public GraphicsContext
	{
	public:
		VulkanContext();
		~VulkanContext();

		virtual void Init(const Shared<SwapChain>& InSwapChain) override;

		virtual Shared<CommandAllocator> GetCommandAllocator() const override { return m_CommandAllocators[m_SwapChain->GetFrameIndex()]; }
		virtual CommandList* GetCurrentCommandList() const override { return m_CommandAllocators[m_SwapChain->GetFrameIndex()]->GetCommandList(0); }
		virtual void ExecuteCommandLists(const std::vector<CommandList*>& InCommandLists) override;

		virtual void NextFrame() override;
		virtual void WaitForGPU() override;

		virtual Shared<DescriptorHeap> GetDescriptorHeap(EDescriptorHeapType InType) const override
		{
			return nullptr;
		}

		uint32_t GetFramesInFlight() const override { return m_SwapChain->GetFrameCount(); }
		uint32_t GetCurrentFrameIndex() const override { return m_SwapChain->GetFrameIndex(); }

		auto GetInstance() { return m_Instance; }
		auto GetInstance() const { return m_Instance; }

		auto GetPhysicalDevice() { return m_PhysicalDevice; }
		auto GetPhysicalDevice() const { return m_PhysicalDevice; }

		auto GetDevice() { return m_Device; }
		auto GetDevice() const { return m_Device; }

		auto GetSurface() { return m_Surface; }
		auto GetSurface() const { return m_Surface; }

		const auto& GetQueueInfo() const { return m_QueueFamily; }

	private:
		void FindSuitablePhysicalDevice();
		std::vector<QueueFamilyInfo> GetDeviceQueuesInfo(VkPhysicalDevice InDevice) const;

	private:
		VkInstance m_Instance = VK_NULL_HANDLE;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkDevice m_Device = VK_NULL_HANDLE;
		QueueFamilyInfo m_QueueFamily;

		Shared<VulkanSwapChain> m_SwapChain = nullptr;

		std::vector<Shared<VulkanCommandAllocator>> m_CommandAllocators;
	};

}
