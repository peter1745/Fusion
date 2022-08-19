#pragma once

#include "VulkanAllocator.h"

namespace Fusion {

	class VulkanIndexBuffer
	{
	public:
		//VulkanIndexBuffer(uint32_t InCount);
		VulkanIndexBuffer(uint32_t InCount, uint32_t* InData);
		~VulkanIndexBuffer();

		void Bind(VkCommandBuffer InCommandBuffer);

		uint32_t GetCount() const { return m_IndexCount; }

	private:
		VkBuffer m_Buffer = VK_NULL_HANDLE;
		VmaAllocation m_BufferAllocation;

		uint32_t m_IndexCount = 0;
	};

}
