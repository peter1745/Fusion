#include "FusionPCH.hpp"
#include "CommandPool.hpp"
#include "GraphicsContext.hpp"

namespace Fusion {

	CommandPool::CommandPool(const Shared<Device>& InDevice, const CommandPoolInfo& InCreateInfo)
	    : m_CreateInfo(InCreateInfo), m_Device(InDevice)
	{
		VkCommandPoolCreateInfo PoolInfo = {};
		PoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		PoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		PoolInfo.queueFamilyIndex = InDevice->GetQueueInfo().QueueFamily;
		FUSION_CORE_VERIFY(vkCreateCommandPool(InDevice->GetLogicalDevice(), &PoolInfo, nullptr, &m_CommandPool) == VK_SUCCESS);

		m_CommandBuffers.resize(InCreateInfo.InitialListCount);
		for (uint32_t Idx = 0; Idx < InCreateInfo.InitialListCount; Idx++)
		{
			VkCommandBufferAllocateInfo AllocInfo = {};
			AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			AllocInfo.commandPool = m_CommandPool;
			AllocInfo.commandBufferCount = 1;
			AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

			VkCommandBuffer CmdBuffer = VK_NULL_HANDLE;
			FUSION_CORE_VERIFY(vkAllocateCommandBuffers(m_Device->GetLogicalDevice(), &AllocInfo, &CmdBuffer) == VK_SUCCESS);

			m_CommandBuffers[Idx] = MakeUnique<CommandBuffer>(CmdBuffer);
		}
	}

	CommandPool::~CommandPool()
	{
		Release();
	}

	CommandBuffer* CommandPool::AllocateCommandBuffer()
	{
		VkCommandBufferAllocateInfo AllocInfo = {};
		AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		AllocInfo.commandPool = m_CommandPool;
		AllocInfo.commandBufferCount = 1;
		AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		VkCommandBuffer CmdBuffer = VK_NULL_HANDLE;
		FUSION_CORE_VERIFY(vkAllocateCommandBuffers(m_Device->GetLogicalDevice(), &AllocInfo, &CmdBuffer) == VK_SUCCESS);

		auto& Result = m_CommandBuffers.emplace_back(MakeUnique<CommandBuffer>(CmdBuffer));
		return Result.get();
	}

	std::vector<CommandBuffer*> CommandPool::AllocateCommandBuffers(size_t InCount)
	{
		std::vector<CommandBuffer*> Result;
		Result.reserve(InCount);
		for (size_t Idx = 0; Idx < InCount; Idx++)
			Result.push_back(AllocateCommandBuffer());
		return Result;
	}

	void CommandPool::DestroyCommandBuffer(CommandBuffer* InCommandBuffer)
	{
		auto It = std::find_if(m_CommandBuffers.begin(), m_CommandBuffers.end(), [InCommandBuffer](const auto& InOther) { return InOther.get() == InCommandBuffer; });
		FUSION_CORE_VERIFY(It != m_CommandBuffers.end(), "CommandBuffer can only be destroyed by the pool that allocated it");

		VkCommandBuffer CmdBuffer = It->get()->GetBuffer();
		vkFreeCommandBuffers(m_Device->GetLogicalDevice(), m_CommandPool, 1, &CmdBuffer);

		m_CommandBuffers.erase(It);
	}

	void CommandPool::Reset()
	{
		vkResetCommandPool(m_Device->GetLogicalDevice(), m_CommandPool, 0);
	}

	void CommandPool::Release()
	{
		if (!m_Device || m_CommandPool == VK_NULL_HANDLE)
			return;

		for (auto& CmdList : m_CommandBuffers)
			CmdList->Release();

		m_CommandBuffers.clear();
		vkDestroyCommandPool(m_Device->GetLogicalDevice(), m_CommandPool, nullptr);

		m_CommandPool = VK_NULL_HANDLE;
		m_Device = nullptr;
	}

}