#pragma once

#include "Fusion/Renderer/IndexBuffer.hpp"

#include "D3D11Common.hpp"

namespace Fusion {

	class D3D11IndexBuffer : public IndexBuffer
	{
	public:
		D3D11IndexBuffer(uint32_t InSize);
		D3D11IndexBuffer(void* InData, uint32_t InSize);
		~D3D11IndexBuffer();

		virtual void Bind() override;

		virtual uint32_t GetCount() const override { return m_Size / sizeof(uint32_t); }

		auto& GetBuffer() { return m_Buffer; }
		const auto& GetBuffer() const { return m_Buffer; }

	private:
		uint32_t m_Size = 0;
		D3DComPtr<ID3D11Buffer> m_Buffer;

	};

}

