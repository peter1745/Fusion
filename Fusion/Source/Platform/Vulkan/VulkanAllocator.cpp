#include "FusionPCH.h"
#include "VulkanAllocator.h"

namespace Fusion {

	struct VulkanAllocatorData
	{
		VmaAllocator Allocator;
		uint32_t TotalAllocatedBytes = 0;
	};
	static VulkanAllocatorData* s_AllocatorData = nullptr;

	VmaAllocation VulkanAllocator::AllocateBuffer(VkBufferCreateInfo* InBufferInfo, VmaMemoryUsage InMemoryUsage, VkMemoryPropertyFlags InMemoryPropertyFlags, VkBuffer* OutBuffer)
	{
		VmaAllocationCreateInfo AllocationCreateInfo = {};
		AllocationCreateInfo.usage = InMemoryUsage;
		AllocationCreateInfo.requiredFlags = InMemoryPropertyFlags;

		VmaAllocation Allocation;
		VmaAllocationInfo AllocationInfo;
		vmaCreateBuffer(s_AllocatorData->Allocator, InBufferInfo, &AllocationCreateInfo, OutBuffer, &Allocation, &AllocationInfo);

		FUSION_CORE_INFO("VulkanAllocator: Allocated buffer of size {}", AllocationInfo.size);
		return Allocation;
	}

	void VulkanAllocator::DestroyBuffer(VkBuffer InBuffer, VmaAllocation InAllocation)
	{
		vmaDestroyBuffer(s_AllocatorData->Allocator, InBuffer, InAllocation);
	}

	void VulkanAllocator::FreeAllocation(VmaAllocation InAllocation)
	{
		vmaFreeMemory(s_AllocatorData->Allocator, InAllocation);
	}

	void VulkanAllocator::MapMemory(VmaAllocation InAllocation, void** OutMemory)
	{
		vmaMapMemory(s_AllocatorData->Allocator, InAllocation, OutMemory);
	}

	void VulkanAllocator::UnmapMemory(VmaAllocation InAllocation)
	{
		vmaUnmapMemory(s_AllocatorData->Allocator, InAllocation);
	}

	void VulkanAllocator::Initialize(VkInstance InInstance, Shared<VulkanDevice> InDevice)
	{
		s_AllocatorData = new VulkanAllocatorData();
		
		VmaAllocatorCreateInfo AllocatorInfo = {};
		AllocatorInfo.vulkanApiVersion = VK_API_VERSION_1_3;
		AllocatorInfo.physicalDevice = InDevice->GetPhysicalDevice();
		AllocatorInfo.device = InDevice->GetLogicalDevice();
		AllocatorInfo.instance = InInstance;

		FUSION_CORE_VERIFY(vmaCreateAllocator(&AllocatorInfo, &s_AllocatorData->Allocator) == VK_SUCCESS);
	}

	void VulkanAllocator::Shutdown()
	{
		vmaDestroyAllocator(s_AllocatorData->Allocator);
		delete s_AllocatorData;
		s_AllocatorData = nullptr;
	}

}
