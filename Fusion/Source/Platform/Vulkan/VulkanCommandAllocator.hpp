#pragma once

#include "Fusion/Renderer/CommandAllocator.hpp"
#include "VulkanDevice.hpp"
#include "VulkanCommandList.hpp"

namespace Fusion {

	class VulkanCommandAllocator : public CommandAllocator
	{
	public:
		VulkanCommandAllocator(const Shared<VulkanDevice>& InDevice, const CommandAllocatorInfo& InCreateInfo);
		~VulkanCommandAllocator() override;

		CommandList* AllocateCommandList() override;
		std::vector<CommandList*> AllocateCommandLists(size_t InCount) override;

		void DestroyCommandList(CommandList* InCommandList) override;

		CommandList* GetCommandList(size_t InIndex) const override
		{
			FUSION_CORE_VERIFY(InIndex < m_CommandLists.size());
			return m_CommandLists[InIndex].get();
		}

		void Reset() override;

		void Release() override;

	private:
		CommandAllocatorInfo m_CreateInfo;
		Shared<VulkanDevice> m_Device = nullptr;
		VkCommandPool m_CommandPool = VK_NULL_HANDLE;
		std::vector<Unique<VulkanCommandList>> m_CommandLists;
	};

}
