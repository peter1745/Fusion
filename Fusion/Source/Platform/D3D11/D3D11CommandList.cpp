#include "FusionPCH.hpp"
#include "D3D11CommandList.hpp"
#include "D3D11CommandAllocator.hpp"

#include "D3D11GraphicsPipeline.hpp"
#include "D3D11Buffer.hpp"
#include "D3D11DescriptorHeap.hpp"

namespace Fusion {

	D3D11CommandList::D3D11CommandList(const Shared<D3D11CommandAllocator>& InAllocator, const D3DComPtr<ID3D11DeviceContext>& InCommandList)
		: m_Allocator(InAllocator), m_DeviceContext(InCommandList)
	{
	}

	void D3D11CommandList::Reset() {}

	void D3D11CommandList::BeginRecording() {}

	void D3D11CommandList::SetViewports(const std::vector<Viewport>& InViewports)
	{
		std::vector<D3D11_VIEWPORT> D3DViewports(InViewports.size());
		std::vector<D3D11_RECT> D3DScissorRects(InViewports.size());
		for (size_t Idx = 0; Idx < InViewports.size(); Idx++)
		{
			const auto& Viewport = InViewports[Idx];
			D3DViewports[Idx] = { Viewport.TopLeftX, Viewport.TopLeftY, Viewport.Width, Viewport.Height, Viewport.MinDepth, Viewport.MaxDepth };
			D3DScissorRects[Idx] = { LONG(0), LONG(0), LONG(Viewport.Width), LONG(Viewport.Height) };
		}

		m_DeviceContext->RSSetViewports(uint32_t(D3DViewports.size()), D3DViewports.data());
		m_DeviceContext->RSSetScissorRects(uint32_t(D3DScissorRects.size()), D3DScissorRects.data());
	}

	void D3D11CommandList::SetConstantBuffer(GraphicsPipeline* InPipeline, uint32_t InIndex, const Shared<Buffer>& InConstantBuffer)
	{
		auto Buffer = InConstantBuffer.As<D3D11Buffer>();

		const auto& LayoutInfo = InPipeline->GetInfo().Layout->GetInfo();
		const auto& Param = LayoutInfo.Parameters[InIndex];
		const auto& ConstantBufferDescriptor = std::get<PipelineLayoutDescriptor>(Param.Value);

		switch (Param.Visibility)
		{
		case EShaderVisibility::All:
			m_DeviceContext->VSSetConstantBuffers(ConstantBufferDescriptor.Space, 1, Buffer->GetResource());
			m_DeviceContext->PSSetConstantBuffers(ConstantBufferDescriptor.Space, 1, Buffer->GetResource());
			break;
		case EShaderVisibility::Vertex:
			m_DeviceContext->VSSetConstantBuffers(ConstantBufferDescriptor.Space, 1, Buffer->GetResource());
			break;
		case EShaderVisibility::Pixel:
			m_DeviceContext->PSSetConstantBuffers(ConstantBufferDescriptor.Space, 1, Buffer->GetResource());
			break;
		}
	}

	void D3D11CommandList::SetVertexBuffer(const VertexBufferView& InBufferView)
	{
		auto Buffer = InBufferView.VertexBuffer.As<D3D11Buffer>();
		UINT Offsets[] = { 0 };
		m_DeviceContext->IASetVertexBuffers(0, 1, Buffer->GetResource(), &InBufferView.VertexStride, Offsets);
	}

	void D3D11CommandList::SetDescriptorHeaps(const std::vector<Shared<DescriptorHeap>>& InHeaps)
	{

	}

	void D3D11CommandList::DrawInstanced(uint32_t InInstanceVertexCount, uint32_t InInstanceCount, uint32_t InStartVertexLocation, uint32_t InStartInstanceLocation)
	{
		m_DeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_DeviceContext->DrawInstanced(InInstanceVertexCount, InInstanceCount, InStartVertexLocation, InStartInstanceLocation);
	}

	void D3D11CommandList::DrawIndexed(const IndexBufferView& InBufferView)
	{
		auto Buffer = InBufferView.IndexBuffer.As<D3D11Buffer>();
		m_DeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_DeviceContext->IASetIndexBuffer(Buffer->GetResource(), EFormatToDXGIFormat(InBufferView.IndexFormat), 0);
		m_DeviceContext->DrawIndexedInstanced(Buffer->GetSize() / GetFormatSize(InBufferView.IndexFormat), 1, 0, 0, 0);
	}

	void D3D11CommandList::EndRecording()
	{

	}

}
