#include "FusionPCH.hpp"
#include "DescriptorHeap.hpp"

#include "Platform/D3D12/D3D12DescriptorHeap.hpp"

namespace Fusion {

	Unique<DescriptorHeap> DescriptorHeap::Create(const DescriptorHeapInfo& InCreateInfo)
	{
		return MakeUnique<D3D12DescriptorHeap>(InCreateInfo);
	}

}
