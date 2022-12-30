#include "FusionPCH.hpp"
#include "D3D12Buffer.hpp"
#include "D3D12Context.hpp"

#include "Fusion/Memory/Utils.h"

namespace Fusion {

	static constexpr D3D12_RESOURCE_STATES EBufferStateToD3D12ResourceState(EBufferState InState)
	{
		uint64_t Result = 0;

		if (InState & BufferStates::Common)
			Result |= D3D12_RESOURCE_STATE_COMMON;
		
		if (InState & BufferStates::Vertex)
			Result |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;

		if (InState & BufferStates::Constant)
			Result |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		
		if (InState & BufferStates::Index)
			Result |= D3D12_RESOURCE_STATE_INDEX_BUFFER;
		
		if (InState & BufferStates::UnorderedAccess)
			Result |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		
		if (InState & BufferStates::NonPixelShaderResource)
			Result |= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		
		if (InState & BufferStates::IndirectArgument)
			Result |= D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
		
		if (InState & BufferStates::CopyDestination)
			Result |= D3D12_RESOURCE_STATE_COPY_DEST;

		if (InState & BufferStates::CopySource)
			Result |= D3D12_RESOURCE_STATE_COPY_SOURCE;
		
		return static_cast<D3D12_RESOURCE_STATES>(Result);
	}

	static constexpr D3D12_HEAP_TYPE HeapTypeToD3D12HeapType(EHeapType InHeapType)
	{
		switch (InHeapType)
		{
		case EHeapType::Default: return D3D12_HEAP_TYPE_DEFAULT;
		case EHeapType::Upload: return D3D12_HEAP_TYPE_UPLOAD;
		case EHeapType::Readback: return D3D12_HEAP_TYPE_READBACK;
		}

		return D3D12_HEAP_TYPE_DEFAULT;
	}

	D3D12Buffer::D3D12Buffer(const BufferInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo)
	{
		/*auto Context = GraphicsContext::Get<D3D12Context>();
		auto& Device = Context->GetDevice();

		D3D12_HEAP_PROPERTIES HeapProperties = {};
		HeapProperties.Type = HeapTypeToD3D12HeapType(InCreateInfo.HeapType);
		HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		HeapProperties.CreationNodeMask = 0;
		HeapProperties.VisibleNodeMask = 0;

		D3D12_RESOURCE_DESC1 BufferDesc = {};
		BufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		BufferDesc.Alignment = 0;
		BufferDesc.Width = Align(InCreateInfo.Size, InCreateInfo.Alignment);
		BufferDesc.Height = 1;
		BufferDesc.DepthOrArraySize = 1;
		BufferDesc.MipLevels = 1;
		BufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		BufferDesc.SampleDesc.Count = 1;
		BufferDesc.SampleDesc.Quality = 0;
		BufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		BufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		BufferDesc.SamplerFeedbackMipRegion.Width = 0;
		BufferDesc.SamplerFeedbackMipRegion.Height = 0;
		BufferDesc.SamplerFeedbackMipRegion.Depth = 0;

		D3D12_RESOURCE_STATES InitialState = EBufferStateToD3D12ResourceState(InCreateInfo.State);
		if (InCreateInfo.HeapType == EHeapType::Upload)
			InitialState = D3D12_RESOURCE_STATE_GENERIC_READ;

		Device->CreateCommittedResource2(&HeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&BufferDesc,
			InitialState,
			nullptr,
			nullptr,
			m_Buffer, m_Buffer
		);

		if (InCreateInfo.InitialData)
		{
			BufferInfo UploadBufferInfo = {};
			UploadBufferInfo.HeapType = EHeapType::Upload;
			UploadBufferInfo.Size = InCreateInfo.Size;
			m_UploadBuffer = Shared<D3D12Buffer>::Create(UploadBufferInfo);

			Shared<D3D12Buffer> Instance = this;
			Context->SubmitToCommandContext([Instance](CommandList* InCmdList) mutable
			{
				Instance->SetData(InCmdList, Instance->m_CreateInfo.InitialData, Instance->m_UploadBuffer);
			});
		}*/
	}

	D3D12Buffer::~D3D12Buffer()
	{

	}

	Byte* D3D12Buffer::Map()
	{
		Byte* BufferStart;
		D3D12_RANGE ReadRange = { 0, 0 };
		m_Buffer->Map(0, &ReadRange, reinterpret_cast<void**>(&BufferStart));
		return BufferStart;
	}

	void D3D12Buffer::Unmap(Byte* InPtr)
	{
		m_Buffer->Unmap(0, nullptr);
	}

	void D3D12Buffer::Transition(CommandList* InCmdList, EBufferState InState)
	{
		if (m_CreateInfo.State == InState)
			return;

		D3D12_RESOURCE_BARRIER Barrier = {};
		Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		Barrier.Transition.pResource = m_Buffer;
		Barrier.Transition.StateBefore = EBufferStateToD3D12ResourceState(m_CreateInfo.State);
		Barrier.Transition.StateAfter = EBufferStateToD3D12ResourceState(InState);
		Barrier.Transition.Subresource = 0;
		static_cast<D3D12CommandList*>(InCmdList)->GetNativeList()->ResourceBarrier(1, &Barrier);

		m_CreateInfo.State = InState;
	}

	void D3D12Buffer::SetData(CommandList* InCmdList, const void* InData, const Shared<Buffer>& InUploadBuffer)
	{
		auto& CmdList = static_cast<D3D12CommandList*>(InCmdList)->GetNativeList();
		auto UploadBuffer = InUploadBuffer.As<D3D12Buffer>();

		Fusion::Byte* Memory = UploadBuffer->Map();
		memcpy(Memory, InData, UploadBuffer->GetSize());
		UploadBuffer->Unmap(Memory);

		D3D12_RESOURCE_BARRIER Barrier = {};
		Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		Barrier.Transition.pResource = m_Buffer;
		Barrier.Transition.StateBefore = EBufferStateToD3D12ResourceState(m_CreateInfo.State);
		Barrier.Transition.StateAfter = EBufferStateToD3D12ResourceState(BufferStates::CopyDestination);
		Barrier.Transition.Subresource = 0;
		CmdList->ResourceBarrier(1, &Barrier);

		CmdList->CopyResource(m_Buffer, UploadBuffer->GetBuffer());

		Barrier.Transition.StateBefore = EBufferStateToD3D12ResourceState(BufferStates::CopyDestination);
		Barrier.Transition.StateAfter = EBufferStateToD3D12ResourceState(m_CreateInfo.State);
		CmdList->ResourceBarrier(1, &Barrier);
	}
}
