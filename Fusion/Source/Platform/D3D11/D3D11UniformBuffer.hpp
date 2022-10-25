#pragma once

#include "Fusion/Renderer/UniformBuffer.hpp"

namespace Fusion {

	class D3D11UniformBuffer : public UniformBuffer
	{
	public:
		D3D11UniformBuffer(uint32_t InSize, EShaderBindPoint InBindPoint, EBufferUsage InUsage);
		D3D11UniformBuffer(void* InData, uint32_t InSize, EShaderBindPoint InBindPoint, EBufferUsage InUsage);
		virtual ~D3D11UniformBuffer();

		virtual void SetData(void* InData) override;

		virtual uint32_t GetSize() const override { return m_Size; }
		virtual EShaderBindPoint GetBindPoint() const override { return m_BindPoint; }

		ID3D11Buffer* GetBuffer() const { return m_Buffer; }

	private:
		ID3D11Buffer* m_Buffer = nullptr;
		uint32_t m_Size = 0;
		EShaderBindPoint m_BindPoint;
	};

}
