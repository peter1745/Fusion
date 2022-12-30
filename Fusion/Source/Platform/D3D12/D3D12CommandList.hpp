#pragma once

#include "D3D12Common.hpp"

#include "Fusion/Renderer/CommandList.hpp"

namespace Fusion {

	class D3D12CommandAllocator;
	class D3D12CommandList : public CommandList
	{
	public:
		D3D12CommandList(const Shared<D3D12CommandAllocator>& InAllocator, const D3DComPtr<ID3D12GraphicsCommandList6>& InCommandList);

		virtual void Reset() override;

		virtual void BeginRecording() override;
		virtual void SetViewports(const std::vector<Viewport>& InViewports) override;

		virtual void SetConstantBuffer(GraphicsPipeline* InPipeline, const std::string& InName, const Shared<Buffer>& InConstantBuffer) override;

		virtual void SetTexture(GraphicsPipeline* InPipeline, const std::string& InName, const Shared<Texture2D>& InTexture) override;
		virtual void SetVertexBuffer(const VertexBufferView& InBufferView) override;

		virtual void SetDescriptorHeaps(const std::vector<Shared<DescriptorHeap>>& InHeaps) override;

		virtual void DrawInstanced(uint32_t InInstanceVertexCount, uint32_t InInstanceCount, uint32_t InStartVertexLocation, uint32_t InStartInstanceLocation) override;
		virtual void DrawIndexed(const IndexBufferView& InBufferView) override;

		virtual void EndRecording() override;

		void Release() override;

		auto& GetNativeList() { return m_CommandList; }

	private:
		Shared<D3D12CommandAllocator> m_Allocator = nullptr;
		D3DComPtr<ID3D12GraphicsCommandList6> m_CommandList;

	private:
		friend class D3D12CommandAllocator;
	};

}
