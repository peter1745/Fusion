#pragma once

#include "Fusion/Renderer/VertexBuffer.h"

namespace Fusion {

	class D3D11VertexBuffer : public VertexBuffer
	{
	public:
		D3D11VertexBuffer(const VertexBufferInfo& InCreateInfo);
		~D3D11VertexBuffer();

		virtual void Resize(uint32_t InNewSize) override;
		virtual void SetData(void* InData, uint32_t InSize) override;
		virtual const VertexBufferLayout& GetLayout() const override { return m_Layout; }

	private:
		VertexBufferLayout m_Layout;
		EBufferUsage m_Usage;
		uint32_t m_Size;

		ID3D11Buffer* m_Buffer = nullptr;
	};

}
