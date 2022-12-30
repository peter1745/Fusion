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

		VmaAllocation AllocateImage(const VkImageCreateInfo& InCreateInfo, VkImage* OutImage);

	private:
		VmaAllocator m_Allocator = VK_NULL_HANDLE;
	};

}
