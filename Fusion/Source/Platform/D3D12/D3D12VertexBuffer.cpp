#include "FusionPCH.hpp"
#include "D3D12VertexBuffer.hpp"

#include "D3D12Context.hpp"

namespace Fusion {

	D3D12VertexBuffer::D3D12VertexBuffer(const VertexBufferInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo)
	{
		BufferInfo BufferCreateInfo = {};
		BufferCreateInfo.HeapType = EHeapType::Upload; // NOTE(Peter): Change this to not be on an upload heap
		BufferCreateInfo.State = 0;
		BufferCreateInfo.Alignment = 0;
		BufferCreateInfo.Size = InCreateInfo.BufferSize;
		BufferCreateInfo.InitialData = InCreateInfo.Data;

		m_Buffer = MakeUnique<D3D12Buffer>(BufferCreateInfo);

		m_VertexBufferView = {};
		m_VertexBufferView.BufferLocation = m_Buffer->GetGPUBufferLocation();
		m_VertexBufferView.SizeInBytes = BufferCreateInfo.Size;
		m_VertexBufferView.StrideInBytes = InCreateInfo.Stride;
	}

	void D3D12VertexBuffer::Bind() const
	{
		auto Context = GraphicsContext::Get<D3D12Context>();
		auto& CmdList = static_cast<D3D12CommandList*>(Context->GetCurrentCommandList())->GetNativeList();
		CmdList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
	}

	void D3D12VertexBuffer::Resize(uint32_t InNewSize)
	{

	}

	void D3D12VertexBuffer::SetData(void* InData, uint32_t InSize)
	{

	}

}
