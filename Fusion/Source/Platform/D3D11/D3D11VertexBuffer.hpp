#pragma once

#include "Fusion/Renderer/VertexBuffer.hpp"

#include "D3D11Common.hpp"

namespace Fusion {

	class D3D11VertexBuffer : public VertexBuffer
	{
	public:
		D3D11VertexBuffer(const VertexBufferInfo& InCreateInfo);
		~D3D11VertexBuffer();

		virtual void Bind() override;
		virtual void Resize(uint32_t InNewSize) override;
		virtual void SetData(void* InData, uint32_t InSize) override;

	private:
		VertexBufferInfo m_CreateInfo;
		D3DComPtr<ID3D11Buffer> m_Buffer;
	};

}
