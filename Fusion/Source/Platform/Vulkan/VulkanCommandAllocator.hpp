#pragma once

#include "Fusion/Renderer/CommandAllocator.hpp"
#include "VulkanQueueFamily.hpp"
#include "VulkanCommandList.hpp"

namespace Fusion {

	class VulkanCommandAllocator : public CommandAllocator
	{
	public:
		explicit VulkanCommandAllocator(const CommandAllocatorInfo& InCreateInfo);
		VulkanCommandAllocator(VkDevice InDevice, const QueueFamilyInfo& InQueueFamily, const CommandAllocatorInfo& InCreateInfo);
		~VulkanCommandAllocator() override;

		CommandList* AllocateCommandList() override;
		CommandList* AllocateCommandList(VkDevice InDevice);

		std::vector<CommandList*> AllocateCommandLists(size_t InCount) override;
		std::vector<CommandList*> AllocateCommandLists(VkDevice InDevice, size_t InCount);

		CommandList* GetCommandList(size_t InIndex) const override
		{
			FUSION_CORE_VERIFY(InIndex < m_CommandLists.size());
			return m_CommandLists[InIndex].get();
		}

		void Reset() override;

	private:
		CommandAllocatorInfo m_CreateInfo;
		VkCommandPool m_CommandPool = VK_NULL_HANDLE;
		std::vector<Unique<VulkanCommandList>> m_CommandLists;
	};

}
