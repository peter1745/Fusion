#include "FusionPCH.hpp"
#include "D3D12Image2D.hpp"
#include "D3D12Context.hpp"

namespace Fusion {

	D3D12Image2D::D3D12Image2D(const Image2DInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo)
	{
		Invalidate();
	}

	void D3D12Image2D::Transition(CommandList* InCmdList, EImageState InState)
	{
		D3D12CommandList* D3DCmdList = static_cast<D3D12CommandList*>(InCmdList);

		D3D12_RESOURCE_BARRIER Barrier = {};
		Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		Barrier.Transition.pResource = m_Resource;
		Barrier.Transition.StateBefore = ImageStatesToD3D12ResourceStates(m_State);
		Barrier.Transition.StateAfter = ImageStatesToD3D12ResourceStates(InState);
		Barrier.Transition.Subresource = 0;
		D3DCmdList->GetNativeList()->ResourceBarrier(1, &Barrier);

		m_State = InState;
	}

	void D3D12Image2D::Invalidate()
	{
		// NOTE(Peter): We may have to transition the image into a safe state.
		//				Or at least wait for this resource to no longer be used by the GPU

		m_Resource.Release();

		D3D12_HEAP_PROPERTIES HeapProperties = {};
		HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		HeapProperties.CreationNodeMask = 0;
		HeapProperties.VisibleNodeMask = 0;

		D3D12_RESOURCE_DESC1 ResourceDesc = {};
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		ResourceDesc.Alignment = 0;
		ResourceDesc.Width = m_CreateInfo.Size.Width;
		ResourceDesc.Height = m_CreateInfo.Size.Height;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.Format = ImageFormatToDXGIFormat(m_CreateInfo.Format);
		ResourceDesc.SampleDesc.Count = 1;
		ResourceDesc.SampleDesc.Quality = 0;
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		ResourceDesc.Flags = ImageFlagsToD3D12ResourceFlags(m_CreateInfo.Flags);

		ResourceDesc.SamplerFeedbackMipRegion.Width = 0;
		ResourceDesc.SamplerFeedbackMipRegion.Height = 0;
		ResourceDesc.SamplerFeedbackMipRegion.Depth = 0;

		D3D12_CLEAR_VALUE ClearValue = {};
		ClearValue.Format = ResourceDesc.Format;

		if (IsDepthFormat(m_CreateInfo.Format))
		{
			ClearValue.DepthStencil.Depth = 1.0f;
			ClearValue.DepthStencil.Stencil = 0.0f;
		}
		else
		{
			memcpy(ClearValue.Color, &m_CreateInfo.ClearColor[0], sizeof(m_CreateInfo.ClearColor));
		}

		GraphicsContext::Get<D3D12Context>()->GetDevice()->CreateCommittedResource2(
			&HeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&ResourceDesc,
			ImageStatesToD3D12ResourceStates(m_State & ~(ImageStates::CopySrc | ImageStates::CopyDst)),
			m_CreateInfo.Usage != EImageUsage::Texture ? &ClearValue : nullptr,
			nullptr,
			m_Resource, m_Resource
		);
	}

}
