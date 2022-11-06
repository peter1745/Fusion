#include "FusionPCH.hpp"
#include "D3D12IndexBuffer.hpp"

namespace Fusion {

	D3D12IndexBuffer::D3D12IndexBuffer(void* InData, uint32_t InSize)
		: m_Size(InSize)
	{
		BufferInfo BufferCreateInfo = {};
		BufferCreateInfo.HeapType = EHeapType::Upload; // NOTE(Peter): Change this to not be on an upload heap
		BufferCreateInfo.State = 0;
		BufferCreateInfo.Alignment = 0;
		BufferCreateInfo.Size = InSize;

		m_Buffer = MakeUnique<D3D12Buffer>(BufferCreateInfo);

		Byte* BufferBegin;
		D3D12_RANGE ReadRange = {};
		ReadRange.Begin = 0;
		ReadRange.End = 0;
		m_Buffer->GetBuffer()->Map(0, &ReadRange, reinterpret_cast<void**>(&BufferBegin));
		memcpy(BufferBegin, InData, InSize);
		m_Buffer->GetBuffer()->Unmap(0, nullptr);
	}

	D3D12IndexBuffer::~D3D12IndexBuffer()
	{

	}

	void D3D12IndexBuffer::Bind()
	{

	}

}
