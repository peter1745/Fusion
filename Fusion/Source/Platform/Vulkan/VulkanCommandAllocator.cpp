#include "FusionPCH.hpp"
#include "VulkanCommandAllocator.hpp"
#include "VulkanContext.hpp"

namespace Fusion {

	VulkanCommandAllocator::VulkanCommandAllocator(const CommandAllocatorInfo& InCreateInfo)
	    : m_CreateInfo(InCreateInfo)
	{
	}

	VulkanCommandAllocator::VulkanCommandAllocator(VkDevice InDevice, const QueueFamilyInfo& InQueueFamily, const CommandAllocatorInfo& InCreateInfo)
	    : m_CreateInfo(InCreateInfo)
	{
		VkCommandPoolCreateInfo PoolInfo = {};
		PoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		PoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // TODO(Peter): Will we ever need to reset command buffers individually?
		PoolInfo.queueFamilyIndex = InQueueFamily.QueueIndex;
		FUSION_CORE_VERIFY(vkCreateCommandPool(InDevice, &PoolInfo, nullptr, &m_CommandPool) == VK_SUCCESS);

		AllocateCommandLists(InDevice, InCreateInfo.InitialListCount);
	}

	VulkanCommandAllocator::~VulkanCommandAllocator()
	{
		m_CommandLists.clear();
		
		vkDestroyCommandPool(GraphicsContext::Get<VulkanContext>()->GetDevice(), m_CommandPool, nullptr);
	}

	CommandList* VulkanCommandAllocator::AllocateCommandList()
	{
		return AllocateCommandList(GraphicsContext::Get<VulkanContext>()->GetDevice());
	}

	CommandList* VulkanCommandAllocator::AllocateCommandList(VkDevice InDevice)
	{
		VkCommandBufferAllocateInfo AllocInfo = {};
		AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		AllocInfo.commandPool = m_CommandPool;
		AllocInfo.commandBufferCount = 1;
		AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		VkCommandBuffer CmdBuffer = VK_NULL_HANDLE;
		FUSION_CORE_VERIFY(vkAllocateCommandBuffers(InDevice, &AllocInfo, &CmdBuffer) == VK_SUCCESS);

		auto& Result = m_CommandLists.emplace_back(MakeUnique<VulkanCommandList>(this, CmdBuffer));
		return Result.get();
	}

	std::vector<CommandList*> VulkanCommandAllocator::AllocateCommandLists(size_t InCount)
	{
		return AllocateCommandLists(GraphicsContext::Get<VulkanContext>()->GetDevice(), InCount);
	}

	std::vector<CommandList*> VulkanCommandAllocator::AllocateCommandLists(VkDevice InDevice, size_t InCount)
	{
		std::vector<CommandList*> Result;
		Result.reserve(InCount);
		for (size_t Idx = 0; Idx < InCount; Idx++)
			Result.push_back(AllocateCommandList(InDevice));
		return Result;
	}

	void VulkanCommandAllocator::Reset()
	{
		vkResetCommandPool(GraphicsContext::Get<VulkanContext>()->GetDevice(), m_CommandPool, 0);
	}

}