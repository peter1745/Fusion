#include "FusionPCH.hpp"
#include "D3D12UniformBuffer.hpp"

#include "D3D12Context.hpp"

#include "Fusion/Memory/Utils.h"

namespace Fusion {

	D3D12UniformBuffer::D3D12UniformBuffer(DescriptorHeap* InHeap, const ConstantBufferInfo& InCreateInfo)
		: m_Heap(InHeap)
	{
		BufferInfo BufferCreateInfo = {};
		BufferCreateInfo.HeapType = EHeapType::Upload; // NOTE(Peter): Change this to not be on an upload heap
		BufferCreateInfo.State = 0;
		BufferCreateInfo.Alignment = 0;
		BufferCreateInfo.Size = Align<256>(InCreateInfo.Size);

		m_Buffer = MakeUnique<D3D12Buffer>(BufferCreateInfo);
		m_Allocation = static_cast<D3D12DescriptorHeap*>(InHeap)->AllocateConstantBufferView(m_Buffer.get(), InCreateInfo.Size);
	}

	D3D12UniformBuffer::~D3D12UniformBuffer()
	{
		m_Heap->Deallocate(m_Allocation.Index);
	}

	void D3D12UniformBuffer::SetData(void* InData)
	{
		Byte* BufferBegin;
		D3D12_RANGE ReadRange = {};
		ReadRange.Begin = 0;
		ReadRange.End = 0;
		m_Buffer->GetBuffer()->Map(0, &ReadRange, reinterpret_cast<void**>(&BufferBegin));
		memcpy(BufferBegin, InData, m_Buffer->GetInfo().Size);
		m_Buffer->GetBuffer()->Unmap(0, nullptr);
	}

	uint32_t D3D12UniformBuffer::GetSize() const
	{
		return 0;
	}

	EShaderBindPoint D3D12UniformBuffer::GetBindPoint() const
	{
		return EShaderBindPoint::Both;
	}

}
