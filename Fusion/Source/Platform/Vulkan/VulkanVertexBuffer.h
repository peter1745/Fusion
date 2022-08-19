#pragma once

#include "VulkanAllocator.h"
#include <vulkan/vulkan.h>

namespace Fusion {

	class VulkanVertexBuffer
	{
	public:
		VulkanVertexBuffer(uint32_t InDataSize);
		VulkanVertexBuffer(uint32_t InDataSize, void* InData);
		~VulkanVertexBuffer();

		void Bind(VkCommandBuffer InCommandBuffer);

	private:
		VkBuffer m_Buffer = VK_NULL_HANDLE;
		VmaAllocation m_BufferAllocation;
	};

}
