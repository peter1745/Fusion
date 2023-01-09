#include "FusionPCH.hpp"
#include "VulkanAllocator.hpp"

namespace Fusion {

	VulkanAllocator::VulkanAllocator(VkInstance InInstance, const Shared<Device>& InDevice)
	{
		VmaAllocatorCreateInfo AllocatorInfo = {};
		AllocatorInfo.instance = InInstance;
		AllocatorInfo.physicalDevice = InDevice->GetPhysicalDevice();
		AllocatorInfo.device = InDevice->GetLogicalDevice();
		AllocatorInfo.vulkanApiVersion = VK_API_VERSION_1_3;
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

	VmaAllocation VulkanAllocator::CreateBuffer(const VkBufferCreateInfo& InCreateInfo, VkBuffer* OutBuffer)
	{
		VmaAllocationCreateInfo AllocationInfo = {};
		AllocationInfo.usage = VMA_MEMORY_USAGE_AUTO;
		AllocationInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

		VmaAllocation Allocation;
		vmaCreateBuffer(m_Allocator, &InCreateInfo, &AllocationInfo, OutBuffer, &Allocation, nullptr);

		return Allocation;
	}

	void* VulkanAllocator::MapMemory(VmaAllocation InAllocation) const
	{
		void* MappedMemory;
		vmaMapMemory(m_Allocator, InAllocation, &MappedMemory);
		return MappedMemory;
	}

	void VulkanAllocator::UnmapMemory(VmaAllocation InAllocation) const
	{
		vmaUnmapMemory(m_Allocator, InAllocation);
	}

	VmaAllocationInfo VulkanAllocator::GetAllocationInfo(VmaAllocation InAllocation) const
	{
		VmaAllocationInfo AllocationInfo;
		vmaGetAllocationInfo(m_Allocator, InAllocation, &AllocationInfo);
		return AllocationInfo;
	}

}
