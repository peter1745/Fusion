#include "FusionPCH.hpp"
#include "D3D12CommandList.hpp"
#include "D3D12CommandAllocator.hpp"

namespace Fusion {

	D3D12CommandList::D3D12CommandList(const Shared<D3D12CommandAllocator>& InAllocator, D3D12ComPtr<ID3D12GraphicsCommandList6> InCommandList)
		: m_Allocator(InAllocator), m_CommandList(InCommandList)
	{
	}

	void D3D12CommandList::Reset()
	{
		m_CommandList->Reset(m_Allocator->GetNativeAllocator(), nullptr);
	}

	void D3D12CommandList::BeginRecording()
	{
	}

	void D3D12CommandList::SetViewports(const std::vector<Viewport>& InViewports)
	{
		std::vector<D3D12_VIEWPORT> D3DViewports(InViewports.size());
		for (size_t Idx = 0; Idx < InViewports.size(); Idx++)
		{
			const auto& Viewport = InViewports[Idx];
			D3DViewports[Idx] = { Viewport.TopLeftX, Viewport.TopLeftY, Viewport.Width, Viewport.Height, Viewport.MinDepth, Viewport.MaxDepth };
		}

		m_CommandList->RSSetViewports(uint32_t(D3DViewports.size()), D3DViewports.data());
	}

	void D3D12CommandList::EndRecording()
	{
		m_CommandList->Close();
	}

}
