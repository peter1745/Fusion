#pragma once

#include "Fusion/Renderer/IndexBuffer.hpp"

#include "D3D12Buffer.hpp"

namespace Fusion {

	class D3D12IndexBuffer : public IndexBuffer
	{
	public:
		D3D12IndexBuffer(void* InData, uint32_t InSize);
		~D3D12IndexBuffer();

		virtual void Bind() override;
		virtual uint32_t GetCount() const override { return m_Size / sizeof(uint32_t); }

		const auto& GetBuffer() const { return m_Buffer; }

	private:
		uint32_t m_Size;

		Unique<D3D12Buffer> m_Buffer;
	};

}
