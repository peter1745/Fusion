#include "FusionPCH.hpp"
#include "D3D12Buffer.hpp"

#include "D3D12Context.hpp"

namespace Fusion {

	static constexpr D3D12_RESOURCE_STATES EBufferStateToD3D12ResourceState(Enum<EBufferState> InState)
	{
		uint64_t Result = 0;

		if ((InState & EBufferState::Common) != 0)
			Result |= D3D12_RESOURCE_STATE_COMMON;
		
		if ((InState & EBufferState::VertexAndConstant) != 0)
			Result |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		
		if ((InState & EBufferState::Index) != 0)
			Result |= D3D12_RESOURCE_STATE_INDEX_BUFFER;
		
		if ((InState & EBufferState::UnorderedAccess) != 0)
			Result |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		
		if ((InState & EBufferState::NonPixelShaderResource) != 0)
			Result |= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		
		if ((InState & EBufferState::IndirectArgument) != 0)
			Result |= D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
		
		if ((InState & EBufferState::CopyDestination) != 0)
			Result |= D3D12_RESOURCE_STATE_COPY_DEST;

		if ((InState & EBufferState::CopySource) != 0)
			Result |= D3D12_RESOURCE_STATE_COPY_SOURCE;
		
		return static_cast<D3D12_RESOURCE_STATES>(Result);
	}

	D3D12Buffer::D3D12Buffer(const BufferInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo)
	{
		auto Context = GraphicsContext::Get<D3D12Context>();
		auto& Device = Context->GetDevice();

		D3D12_HEAP_PROPERTIES HeapProperties = {};
		HeapProperties.Type = InCreateInfo.HeapType == EHeapType::Default ? D3D12_HEAP_TYPE_DEFAULT : D3D12_HEAP_TYPE_UPLOAD;
		HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		HeapProperties.CreationNodeMask = 0;
		HeapProperties.VisibleNodeMask = 0;

		D3D12_RESOURCE_DESC1 BufferDesc = {};
		BufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		BufferDesc.Alignment = InCreateInfo.Alignment;
		BufferDesc.Width = InCreateInfo.Size;
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

		if (InCreateInfo.InitialData && InCreateInfo.HeapType == EHeapType::Upload)
		{
			Byte* BufferBegin;
			D3D12_RANGE ReadRange = {};
			ReadRange.Begin = 0;
			ReadRange.End = 0;
			m_Buffer->Map(0, &ReadRange, reinterpret_cast<void**>(&BufferBegin));
			memcpy(BufferBegin, InCreateInfo.InitialData, InCreateInfo.Size);
			m_Buffer->Unmap(0, nullptr);
		}
	}

	D3D12Buffer::~D3D12Buffer()
	{

	}

}
