#pragma once

#include "Fusion/Renderer/IndexBuffer.hpp"

namespace Fusion {

	class D3D11IndexBuffer : public IndexBuffer
	{
	public:
		D3D11IndexBuffer(uint32_t InSize);
		D3D11IndexBuffer(void* InData, uint32_t InSize);
		~D3D11IndexBuffer();

		virtual void Bind() const override;

		virtual uint32_t GetCount() const override { return m_Size / sizeof(uint32_t); }

	private:
		uint32_t m_Size = 0;
		ID3D11Buffer* m_Buffer = nullptr;

	};

}

