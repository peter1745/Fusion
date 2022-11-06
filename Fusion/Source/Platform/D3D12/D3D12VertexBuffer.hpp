#pragma once

#include "Fusion/Renderer/VertexBuffer.hpp"

#include "D3D12Buffer.hpp"

namespace Fusion {

	class D3D12VertexBuffer : public VertexBuffer
	{
	public:
		D3D12VertexBuffer(const VertexBufferInfo& InCreateInfo);

		virtual void Bind() override;
		virtual void Resize(uint32_t InNewSize) override;
		virtual void SetData(void* InData, uint32_t InSize) override;

	private:
		VertexBufferInfo m_CreateInfo;
		Unique<D3D12Buffer> m_Buffer;
		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
	};

}
