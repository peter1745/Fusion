#pragma once

#include "Fusion/Core/UUID.hpp"

#include "VulkanDevice.hpp"

#include <VMA/vk_mem_alloc.h>

namespace Fusion {

	class VulkanAllocator
	{
	public:
		VulkanAllocator(VkInstance InInstance, const Shared<VulkanDevice>& InDevice);
		~VulkanAllocator();

		VmaAllocation CreateImage(const VkImageCreateInfo& InCreateInfo, VkImage* OutImage);
		void DestroyImage(VmaAllocation InAllocation, VkImage InImage);

		VmaAllocation CreateBuffer(const VkBufferCreateInfo& InCreateInfo, VkBuffer* OutBuffer);

		void* MapMemory(VmaAllocation InAllocation) const;
		void UnmapMemory(VmaAllocation InAllocation) const;

		VmaAllocationInfo GetAllocationInfo(VmaAllocation InAllocation) const;

	private:
		VmaAllocator m_Allocator = VK_NULL_HANDLE;
	};

}
