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

	private:
		VmaAllocator m_Allocator = VK_NULL_HANDLE;
	};

}
