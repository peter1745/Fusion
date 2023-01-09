#include "FusionPCH.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanDevice.hpp"

namespace Fusion {

	VulkanBuffer::VulkanBuffer(const BufferInfo& InInfo)
	    : m_Info(InInfo)
	{
		auto VKContext = GraphicsContext::Get<VulkanContext>();

		auto VKDevice = VKContext->GetDevice().As<VulkanDevice>();
		uint32_t QueueFamily = VKDevice->GetQueueInfo().QueueFamily;

		VkBufferCreateInfo BufferCreateInfo = {};
		BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		BufferCreateInfo.pNext = nullptr;
		BufferCreateInfo.flags = 0;
		BufferCreateInfo.size = InInfo.Size;
		BufferCreateInfo.usage = BufferUsageToVkBufferUsageFlags(InInfo.State);
		BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		BufferCreateInfo.queueFamilyIndexCount = 1;
		BufferCreateInfo.pQueueFamilyIndices = &QueueFamily;
		m_Allocation = VKContext->GetAllocator()->CreateBuffer(BufferCreateInfo, &m_Buffer);

		if (InInfo.InitialData)
		{
			void* Memory = VKContext->GetAllocator()->MapMemory(m_Allocation);
			memcpy(Memory, InInfo.InitialData, InInfo.Size);
			VKContext->GetAllocator()->UnmapMemory(m_Allocation);
		}
	}

	VulkanBuffer::~VulkanBuffer()
	{
	}

	Byte* VulkanBuffer::Map()
	{
		return nullptr;
	}

	void VulkanBuffer::Unmap(Byte* InPtr)
	{
	}

	void VulkanBuffer::Transition(CommandList* InCmdList, EBufferState InState)
	{
	}

	void VulkanBuffer::SetData(CommandList* InCmdList, const void* InData, const Shared<Buffer>& InUploadBuffer)
	{
	}

}
