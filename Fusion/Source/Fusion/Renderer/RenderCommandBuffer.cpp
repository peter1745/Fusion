#include "FusionPCH.h"
#include "RenderCommandBuffer.h"

namespace Fusion {

	RenderCommandBuffer::RenderCommandBuffer()
	{
		m_CommandBuffer = new Byte[10 * 1024];
		m_CommandBufferPtr = m_CommandBuffer;
	}

	RenderCommandBuffer::~RenderCommandBuffer()
	{
		m_CommandBuffer = nullptr;
		delete[] m_CommandBuffer;
	}

	void RenderCommandBuffer::Execute()
	{
		Byte* Buffer = m_CommandBuffer;

		for (uint32_t i = 0; i < m_CommandCount; i++)
		{
			RenderCommandFunc CommandFunc = *(RenderCommandFunc*)Buffer;
			Buffer += sizeof(RenderCommandFunc);

			uint32_t Size = *(uint32_t*)Buffer;
			Buffer += sizeof(uint32_t);

			CommandFunc(Buffer);

			Buffer += Size;
		}

		m_CommandBufferPtr = m_CommandBuffer;
		m_CommandCount = 0;
	}

	void* RenderCommandBuffer::Allocate(RenderCommandFunc InFunc, uint32_t InSize)
	{
		*(RenderCommandFunc*)m_CommandBufferPtr = InFunc;
		m_CommandBufferPtr += sizeof(RenderCommandFunc);

		*(uint32_t*)m_CommandBufferPtr = InSize;
		m_CommandBufferPtr += sizeof(uint32_t);

		void* Memory = m_CommandBufferPtr;
		m_CommandBufferPtr += InSize;

		m_CommandCount++;
		return Memory;
	}

}
