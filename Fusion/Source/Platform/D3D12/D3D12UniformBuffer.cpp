#include "FusionPCH.hpp"
#include "D3D12UniformBuffer.hpp"

#include "D3D12Context.hpp"

namespace Fusion {

	D3D12UniformBuffer::D3D12UniformBuffer(DescriptorHeap* InHeap, const ConstantBufferInfo& InCreateInfo)
		: m_Heap(InHeap)
	{
		BufferInfo BufferCreateInfo = {};
		BufferCreateInfo.HeapType = EHeapType::Upload; // NOTE(Peter): Change this to not be on an upload heap
		BufferCreateInfo.State = 0;
		BufferCreateInfo.Alignment = 0;
		BufferCreateInfo.Size = InCreateInfo.Size;

		m_Buffer = MakeUnique<D3D12Buffer>(BufferCreateInfo);

		D3D12_CONSTANT_BUFFER_VIEW_DESC ViewDesc = {};
		ViewDesc.BufferLocation = m_Buffer->GetGPUBufferLocation();
		ViewDesc.SizeInBytes = InCreateInfo.Size;

		m_Allocation = static_cast<D3D12DescriptorHeap*>(InHeap)->AllocateConstantBufferView(m_Buffer.get(), InCreateInfo.Size);
	}

	D3D12UniformBuffer::~D3D12UniformBuffer()
	{
		m_Heap->Deallocate(m_Allocation.Index);
	}

	void D3D12UniformBuffer::SetData(void* InData)
	{

	}

	uint32_t D3D12UniformBuffer::GetSize() const
	{

	}

	EShaderBindPoint D3D12UniformBuffer::GetBindPoint() const
	{

	}

}
