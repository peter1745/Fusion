#include "FusionPCH.hpp"
#include "VulkanAllocator.hpp"

namespace Fusion {

	VulkanAllocator::VulkanAllocator(VkInstance InInstance, const Shared<VulkanDevice>& InDevice)
	{
		VmaAllocatorCreateInfo AllocatorInfo = {};
		AllocatorInfo.instance = InInstance;
		AllocatorInfo.physicalDevice = InDevice->GetPhysicalDevice();
		AllocatorInfo.device = InDevice->GetLogicalDevice();
		AllocatorInfo.vulkanApiVersion = VK_API_VERSION_1_2;
		AllocatorInfo.preferredLargeHeapBlockSize = 0;
		vmaCreateAllocator(&AllocatorInfo, &m_Allocator);
	}

	VulkanAllocator::~VulkanAllocator()
	{
	}

	VmaAllocation VulkanAllocator::CreateImage(const VkImageCreateInfo& InCreateInfo, VkImage* OutImage)
	{
		VmaAllocationCreateInfo AllocationInfo = {};
		AllocationInfo.usage = VMA_MEMORY_USAGE_AUTO;

		VmaAllocation Allocation;
		vmaCreateImage(m_Allocator, &InCreateInfo, &AllocationInfo, OutImage, &Allocation, nullptr);

		return Allocation;
	}

	void VulkanAllocator::DestroyImage(VmaAllocation InAllocation, VkImage InImage)
	{
		vmaDestroyImage(m_Allocator, InImage, InAllocation);
	}

}
