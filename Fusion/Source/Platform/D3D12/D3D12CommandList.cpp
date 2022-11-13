#include "FusionPCH.hpp"
#include "D3D12CommandList.hpp"
#include "D3D12CommandAllocator.hpp"
#include "D3D12Context.hpp"
#include "D3D12Buffer.hpp"
#include "D3D12DescriptorHeap.hpp"
#include "D3D12GraphicsPipeline.hpp"
#include "D3D12Texture.hpp"

namespace Fusion {

	D3D12CommandList::D3D12CommandList(const Shared<D3D12CommandAllocator>& InAllocator, const D3DComPtr<ID3D12GraphicsCommandList6>& InCommandList)
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
		std::vector<D3D12_RECT> D3DScissorRects(InViewports.size());
		for (size_t Idx = 0; Idx < InViewports.size(); Idx++)
		{
			const auto& Viewport = InViewports[Idx];
			D3DViewports[Idx] = { Viewport.TopLeftX, Viewport.TopLeftY, Viewport.Width, Viewport.Height, Viewport.MinDepth, Viewport.MaxDepth };
			D3DScissorRects[Idx] = { LONG(0), LONG(0), LONG(Viewport.Width), LONG(Viewport.Height)};
		}

		m_CommandList->RSSetViewports(uint32_t(D3DViewports.size()), D3DViewports.data());
		m_CommandList->RSSetScissorRects(uint32_t(D3DScissorRects.size()), D3DScissorRects.data());
	}

	void D3D12CommandList::SetConstantBuffer(GraphicsPipeline* InPipeline, const std::string& InName, const Shared<Buffer>& InConstantBuffer)
	{
		auto ConstantBuffer = InConstantBuffer.As<D3D12Buffer>();

		const auto& ResourceInfo = InPipeline->GetResourceInfo(InName);
		m_CommandList->SetGraphicsRootConstantBufferView(ResourceInfo.BindingPoint, ConstantBuffer->GetGPUBufferLocation());
	}

	void D3D12CommandList::SetTexture(GraphicsPipeline* InPipeline, const std::string& InName, const Shared<Texture2D>& InTexture)
	{
		auto Image = InTexture->GetImage();
		const auto& SRVAllocation = InTexture.As<D3D12Texture2D>()->GetShaderResourceView();

		if (SRVAllocation.Heap == nullptr || !(Image->GetState() & ImageStates::PixelShaderResource))
			return;

		const auto& ResourceInfo = InPipeline->GetResourceInfo(InName);

		D3D12_GPU_DESCRIPTOR_HANDLE Handle = { static_cast<D3D12DescriptorHeap*>(SRVAllocation.Heap)->GetGPUDescriptorHandle(SRVAllocation)};
		m_CommandList->SetGraphicsRootDescriptorTable(ResourceInfo.BindingPoint, Handle);
	}

	void D3D12CommandList::SetVertexBuffer(const VertexBufferView& InBufferView)
	{
		auto Buffer = InBufferView.VertexBuffer.As<D3D12Buffer>();
		D3D12_VERTEX_BUFFER_VIEW VertexBufferView = {};
		VertexBufferView.BufferLocation = Buffer->GetGPUBufferLocation();
		VertexBufferView.SizeInBytes = Buffer->GetSize();
		VertexBufferView.StrideInBytes = InBufferView.VertexStride;
		m_CommandList->IASetVertexBuffers(0, 1, &VertexBufferView);
	}

	void D3D12CommandList::SetDescriptorHeaps(const std::vector<Shared<DescriptorHeap>>& InHeaps)
	{
		std::vector<ID3D12DescriptorHeap*> D3DHeaps(InHeaps.size());
		for (size_t Idx = 0; Idx < InHeaps.size(); Idx++)
			D3DHeaps[Idx] = InHeaps[Idx].As<D3D12DescriptorHeap>()->GetHeap();

		m_CommandList->SetDescriptorHeaps(D3DHeaps.size(), D3DHeaps.data());
	}

	void D3D12CommandList::DrawInstanced(uint32_t InInstanceVertexCount, uint32_t InInstanceCount, uint32_t InStartVertexLocation, uint32_t InStartInstanceLocation)
	{
		m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_CommandList->DrawInstanced(InInstanceVertexCount, InInstanceCount, InStartVertexLocation, InStartInstanceLocation);
	}

	void D3D12CommandList::DrawIndexed(const IndexBufferView& InBufferView)
	{
		auto Buffer = InBufferView.IndexBuffer.As<D3D12Buffer>();

		// TODO(Peter): This should be part of the pipeline binding
		m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		D3D12_INDEX_BUFFER_VIEW IndexBufferView = {};
		IndexBufferView.BufferLocation = Buffer->GetGPUBufferLocation();
		IndexBufferView.SizeInBytes = Buffer->GetSize();
		IndexBufferView.Format = EFormatToDXGIFormat(InBufferView.IndexFormat);
		m_CommandList->IASetIndexBuffer(&IndexBufferView);

		m_CommandList->DrawIndexedInstanced(Buffer->GetSize() / GetFormatSize(InBufferView.IndexFormat), 1, 0, 0, 0);
	}

	void D3D12CommandList::EndRecording()
	{
		m_CommandList->Close();
	}

}
