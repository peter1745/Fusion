#include "FusionPCH.hpp"
#include "Buffer.hpp"
#include "Device.hpp"
#include "GraphicsContext.hpp"

namespace Fusion {

	Buffer::Buffer(const BufferInfo& InInfo)
	    : m_Info(InInfo)
	{
		uint32_t QueueFamily = GraphicsContext::Get()->GetDevice()->GetQueueInfo().QueueFamily;

		VkBufferCreateInfo BufferCreateInfo = {};
		BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		BufferCreateInfo.pNext = nullptr;
		BufferCreateInfo.flags = 0;
		BufferCreateInfo.size = InInfo.Size;
		BufferCreateInfo.usage = BufferUsageToVkBufferUsageFlags(InInfo.Usage);
		BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		BufferCreateInfo.queueFamilyIndexCount = 1;
		BufferCreateInfo.pQueueFamilyIndices = &QueueFamily;
		m_Allocation = GraphicsContext::Get()->GetAllocator()->CreateBuffer(BufferCreateInfo, &m_Buffer);

		if (InInfo.InitialData)
		{
			void* Memory = GraphicsContext::Get()->GetAllocator()->MapMemory(m_Allocation);
			memcpy(Memory, InInfo.InitialData, InInfo.Size);
			GraphicsContext::Get()->GetAllocator()->UnmapMemory(m_Allocation);
		}
	}

	Buffer::~Buffer()
	{
	}

	void* Buffer::Map()
	{
		return GraphicsContext::Get()->GetAllocator()->MapMemory(m_Allocation);
	}

	void Buffer::Unmap(Byte* InPtr)
	{
		GraphicsContext::Get()->GetAllocator()->UnmapMemory(m_Allocation);
	}

	/*void Buffer::Transition(CommandList* InCmdList, EBufferState InState)
	{
	}*/

	void Buffer::SetData(CommandBuffer* InCmdList, const void* InData, const Shared<Buffer>& InUploadBuffer)
	{
	}

}
