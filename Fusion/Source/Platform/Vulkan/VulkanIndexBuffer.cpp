#include "FusionPCH.h"
#include "VulkanIndexBuffer.h"
#include "VulkanContext.h"
#include "VulkanRenderer.h"

namespace Fusion {

	/*VulkanIndexBuffer::VulkanIndexBuffer(uint32_t InCount)
	{

	}*/

	VulkanIndexBuffer::VulkanIndexBuffer(uint32_t InCount, uint32_t* InData)
		: m_IndexCount(InCount)
	{
		VkBufferCreateInfo StagingBufferInfo = {};
		StagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		StagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		StagingBufferInfo.size = InCount * sizeof(uint32_t);
		StagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkBuffer StagingBuffer;
		VmaAllocation StagingBufferAllocation = VulkanAllocator::AllocateBuffer(&StagingBufferInfo, VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &StagingBuffer);

		void* mappedMemory;
		VulkanAllocator::MapMemory(StagingBufferAllocation, &mappedMemory);
		memcpy(mappedMemory, InData, InCount * sizeof(uint32_t));
		VulkanAllocator::UnmapMemory(StagingBufferAllocation);

		VkBufferCreateInfo BufferInfo = {};
		BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		BufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		BufferInfo.size = InCount * sizeof(uint32_t);
		BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		m_BufferAllocation = VulkanAllocator::AllocateBuffer(&BufferInfo, VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &m_Buffer);

		{
			Shared<VulkanDevice> Device = VulkanContext::Get().GetDevice();
			VkCommandPoolCreateInfo CommandPoolCreateInfo = {};
			CommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			CommandPoolCreateInfo.flags = 0;
			CommandPoolCreateInfo.queueFamilyIndex = Device->GetQueueIndicies().GraphicsQueue;

			VkCommandPool CommandPool;
			FUSION_CORE_VERIFY(vkCreateCommandPool(Device->GetLogicalDevice(), &CommandPoolCreateInfo, nullptr, &CommandPool) == VK_SUCCESS);

			VkCommandBufferAllocateInfo CommandBufferAllocateInfo = {};
			CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			CommandBufferAllocateInfo.commandPool = CommandPool;
			CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			CommandBufferAllocateInfo.commandBufferCount = 1;

			VkCommandBuffer CommandBuffer;
			FUSION_CORE_VERIFY(vkAllocateCommandBuffers(Device->GetLogicalDevice(), &CommandBufferAllocateInfo, &CommandBuffer) == VK_SUCCESS);

			VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
			CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			CommandBufferBeginInfo.flags = 0;
			FUSION_CORE_VERIFY(vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo) == VK_SUCCESS);
			VkBufferCopy BufferCopyInfo = {};
			BufferCopyInfo.srcOffset = 0;
			BufferCopyInfo.dstOffset = 0;
			BufferCopyInfo.size = InCount * sizeof(uint32_t);
			vkCmdCopyBuffer(CommandBuffer, StagingBuffer, m_Buffer, 1, &BufferCopyInfo);
			FUSION_CORE_VERIFY(vkEndCommandBuffer(CommandBuffer) == VK_SUCCESS);

			// TODO(Peter): Use proper synchronization instead of waiting for idle (hangs GPU)

			VkSubmitInfo SubmitInfo = {};
			SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			SubmitInfo.commandBufferCount = 1;
			SubmitInfo.pCommandBuffers = &CommandBuffer;
			vkQueueSubmit(Device->GetGraphicsQueue(), 1, &SubmitInfo, VK_NULL_HANDLE);
			vkQueueWaitIdle(Device->GetGraphicsQueue());

			vkFreeCommandBuffers(Device->GetLogicalDevice(), CommandPool, 1, &CommandBuffer);
			vkDestroyCommandPool(Device->GetLogicalDevice(), CommandPool, nullptr);
		}

		VulkanAllocator::DestroyBuffer(StagingBuffer, StagingBufferAllocation);
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		VulkanAllocator::DestroyBuffer(m_Buffer, m_BufferAllocation);
	}

	void VulkanIndexBuffer::Bind(VkCommandBuffer InCommandBuffer)
	{
		vkCmdBindIndexBuffer(InCommandBuffer, m_Buffer, 0, VK_INDEX_TYPE_UINT32);
	}

}
