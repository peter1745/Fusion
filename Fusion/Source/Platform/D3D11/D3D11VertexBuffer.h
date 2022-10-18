#pragma once

#include "Fusion/Renderer/VertexBuffer.h"

namespace Fusion {

	class D3D11VertexBuffer : public VertexBuffer
	{
	public:
		D3D11VertexBuffer(const VertexBufferInfo& InCreateInfo);
		~D3D11VertexBuffer();

		virtual void Bind() const override;
		virtual void Resize(uint32_t InNewSize) override;
		virtual void SetData(void* InData, uint32_t InSize) override;

	private:
		VertexBufferInfo m_CreateInfo;

		ID3D11Buffer* m_Buffer = nullptr;
	};

}
