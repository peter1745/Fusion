#pragma once

#include "Fusion/Renderer/Renderer.hpp"

#include "VulkanContext.hpp"
#include "VulkanSwapChain.hpp"

namespace Fusion {

	class VulkanRenderer : public Renderer
	{
	public:
		VulkanRenderer(const RendererInfo& InInfo);
		~VulkanRenderer() = default;

		void BeginFrame() override;
		void EndFrame() override;

		void ExecuteCommandLists(const std::vector<CommandList*>& InCommandLists) override;

		Shared<CommandAllocator> GetCurrentCommandAllocator() const override { return m_CommandAllocators[m_CurrentFrame]; }
		CommandList* GetCurrentCommandList() const override { return m_CommandAllocators[m_CurrentFrame]->GetCommandList(0); }

		[[nodiscard]] uint32_t GetFramesInFlight() const override { return m_FramesInFlight; }
		[[nodiscard]] uint32_t GetCurrentFrame() const override { return m_CurrentFrame; }

		void Release() override;

	private:
		Shared<VulkanContext> m_Context = nullptr;
		Shared<VulkanSwapChain> m_SwapChain = nullptr;

		uint32_t m_CurrentFrame = 0;
		uint32_t m_FramesInFlight = 3;

		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkFence> m_ImageFences;

		std::vector<Shared<VulkanCommandAllocator>> m_CommandAllocators;
	};

}
