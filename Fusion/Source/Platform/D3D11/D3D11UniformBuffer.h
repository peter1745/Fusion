#pragma once

#include "Fusion/Renderer/UniformBuffer.h"

namespace Fusion {

	class D3D11UniformBuffer : public UniformBuffer
	{
	public:
		D3D11UniformBuffer(uint32_t InSize, EBufferUsage InUsage);
		D3D11UniformBuffer(void* InData, uint32_t InSize, EBufferUsage InUsage);
		virtual ~D3D11UniformBuffer();

		virtual void SetData(void* InData) override;

		virtual uint32_t GetSize() const override { return m_Size; }

	private:
		ID3D11Buffer* m_Buffer = nullptr;
		uint32_t m_Size = 0;
	};

}
