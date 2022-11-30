#pragma once

#include "Fusion/Renderer/GraphicsContext.hpp"

namespace Fusion {

	class VulkanContext : public GraphicsContext
	{
	public:
		VulkanContext();
		~VulkanContext();

		virtual Shared<CommandAllocator> GetCommandAllocator() const override { return nullptr; }
		virtual CommandList* GetCurrentCommandList() const override { return nullptr; }
		virtual void ExecuteCommandLists(const std::vector<CommandList*>& InCommandLists) override;

		virtual void NextFrame() override;
		virtual void WaitForGPU() override;

		virtual Shared<DescriptorHeap> GetDescriptorHeap(EDescriptorHeapType InType) const override
		{
			return nullptr;
		}

		virtual uint32_t GetCurrentFrameIndex() const override { return 0; }
		uint32_t GetFramesInFlight() const { return 0; }

	private:
	};

}
