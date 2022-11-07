#pragma once

#include "Fusion/Renderer/Buffer.hpp"

#include "D3D11Common.hpp"

namespace Fusion {

	class D3D11Buffer : public Buffer
	{
	public:
		D3D11Buffer(const BufferInfo& InCreateInfo);
		~D3D11Buffer();

		virtual Byte* Map() override;
		virtual void Unmap(Byte* InPtr) override;

		virtual void Transition(CommandList* InCmdList, EBufferState InState) override;

		virtual void SetData(CommandList* InCmdList, const void* InData, const Shared<Buffer>& InUploadBuffer) override;

		virtual const BufferInfo& GetInfo() const override { return m_CreateInfo; }
		virtual uint64_t GetSize() const override { return m_CreateInfo.Size; }

		auto& GetResource() { return m_Buffer; }
		const auto& GetResource() const { return m_Buffer; }

	private:
		BufferInfo m_CreateInfo;
		D3DComPtr<ID3D11Buffer> m_Buffer;
	};

}
