#include "FusionPCH.hpp"
#include "VulkanCommandAllocator.hpp"
#include "VulkanContext.hpp"

namespace Fusion {

	VulkanCommandAllocator::VulkanCommandAllocator(const Shared<VulkanDevice>& InDevice, const CommandAllocatorInfo& InCreateInfo)
	    : m_CreateInfo(InCreateInfo), m_Device(InDevice)
	{
		VkCommandPoolCreateInfo PoolInfo = {};
		PoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		PoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		PoolInfo.queueFamilyIndex = InDevice->GetQueueInfo().QueueFamily;
		FUSION_CORE_VERIFY(vkCreateCommandPool(InDevice->GetLogicalDevice(), &PoolInfo, nullptr, &m_CommandPool) == VK_SUCCESS);

		m_CommandLists.resize(InCreateInfo.InitialListCount);
		for (uint32_t Idx = 0; Idx < InCreateInfo.InitialListCount; Idx++)
		{
			VkCommandBufferAllocateInfo AllocInfo = {};
			AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			AllocInfo.commandPool = m_CommandPool;
			AllocInfo.commandBufferCount = 1;
			AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

			VkCommandBuffer CmdBuffer = VK_NULL_HANDLE;
			FUSION_CORE_VERIFY(vkAllocateCommandBuffers(m_Device->GetLogicalDevice(), &AllocInfo, &CmdBuffer) == VK_SUCCESS);

			m_CommandLists[Idx] = MakeUnique<VulkanCommandList>(CmdBuffer);
		}
	}

	VulkanCommandAllocator::~VulkanCommandAllocator()
	{
		Release();
	}

	CommandList* VulkanCommandAllocator::AllocateCommandList()
	{
		VkCommandBufferAllocateInfo AllocInfo = {};
		AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		AllocInfo.commandPool = m_CommandPool;
		AllocInfo.commandBufferCount = 1;
		AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		VkCommandBuffer CmdBuffer = VK_NULL_HANDLE;
		FUSION_CORE_VERIFY(vkAllocateCommandBuffers(m_Device->GetLogicalDevice(), &AllocInfo, &CmdBuffer) == VK_SUCCESS);

		auto& Result = m_CommandLists.emplace_back(MakeUnique<VulkanCommandList>(CmdBuffer));
		return Result.get();
	}

	std::vector<CommandList*> VulkanCommandAllocator::AllocateCommandLists(size_t InCount)
	{
		std::vector<CommandList*> Result;
		Result.reserve(InCount);
		for (size_t Idx = 0; Idx < InCount; Idx++)
			Result.push_back(AllocateCommandList());
		return Result;
	}

	void VulkanCommandAllocator::DestroyCommandList(CommandList* InCommandList)
	{
		auto It = std::find_if(m_CommandLists.begin(), m_CommandLists.end(), [InCommandList](const auto& InOther) { return InOther.get() == InCommandList; });
		FUSION_CORE_VERIFY(It != m_CommandLists.end(), "CommandList can only be destroyed by the pool that allocated it");

		VkCommandBuffer CmdBuffer = It->get()->GetBuffer();
		vkFreeCommandBuffers(m_Device->GetLogicalDevice(), m_CommandPool, 1, &CmdBuffer);

		m_CommandLists.erase(It);
	}

	void VulkanCommandAllocator::Reset()
	{
		vkResetCommandPool(m_Device->GetLogicalDevice(), m_CommandPool, 0);
	}

	void VulkanCommandAllocator::Release()
	{
		if (!m_Device || m_CommandPool == VK_NULL_HANDLE)
			return;

		for (auto& CmdList : m_CommandLists)
			CmdList->Release();

		m_CommandLists.clear();
		vkDestroyCommandPool(m_Device->GetLogicalDevice(), m_CommandPool, nullptr);

		m_CommandPool = VK_NULL_HANDLE;
		m_Device = nullptr;
	}

}