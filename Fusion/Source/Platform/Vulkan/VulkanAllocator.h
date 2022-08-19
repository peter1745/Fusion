#pragma once

#include "VulkanDevice.h"
#include "VMA/vk_mem_alloc.h"

namespace Fusion {

	class VulkanAllocator
	{
	public:
		VulkanAllocator() = delete;
		~VulkanAllocator() = delete;

		static VmaAllocation AllocateBuffer(VkBufferCreateInfo* InBufferInfo, VmaMemoryUsage InMemoryUsage, VkMemoryPropertyFlags InMemoryPropertyFlags, VkBuffer* OutBuffer);
		static void DestroyBuffer(VkBuffer InBuffer, VmaAllocation InAllocation);
		static void FreeAllocation(VmaAllocation InAllocation);

		static void MapMemory(VmaAllocation InAllocation, void** OutMemory);
		static void UnmapMemory(VmaAllocation InAllocation);

	private:
		static void Initialize(VkInstance InInstance, Shared<VulkanDevice> InDevice);
		static void Shutdown();

		friend class VulkanContext;
	};

}
