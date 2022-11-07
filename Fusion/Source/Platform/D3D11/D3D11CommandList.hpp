#pragma once

#include "Fusion/Renderer/CommandList.hpp"

#include "D3D11Common.hpp"

namespace Fusion {

	class D3D11CommandAllocator;
	class D3D11CommandList : public CommandList
	{
	public:
		D3D11CommandList(const Shared<D3D11CommandAllocator>& InAllocator, const D3DComPtr<ID3D11DeviceContext>& InCommandList);

		virtual void Reset() override;

		virtual void BeginRecording() override;
		virtual void SetViewports(const std::vector<Viewport>& InViewports) override;

		virtual void SetConstantBuffer(GraphicsPipeline* InPipeline, uint32_t InIndex, const Shared<Buffer>& InConstantBuffer) override;
		virtual void SetVertexBuffer(const VertexBufferView& InBufferView) override;

		virtual void SetDescriptorHeaps(const std::vector<Shared<DescriptorHeap>>& InHeaps) override;

		virtual void DrawInstanced(uint32_t InInstanceVertexCount, uint32_t InInstanceCount, uint32_t InStartVertexLocation, uint32_t InStartInstanceLocation) override;
		virtual void DrawIndexed(const IndexBufferView& InBufferView) override;

		virtual void EndRecording() override;

		auto& GetNativeList() { return m_DeviceContext; }
		const auto& GetNativeList() const { return m_DeviceContext; }

	private:
		Shared<D3D11CommandAllocator> m_Allocator = nullptr;
		D3DComPtr<ID3D11DeviceContext> m_DeviceContext;
	};

}
