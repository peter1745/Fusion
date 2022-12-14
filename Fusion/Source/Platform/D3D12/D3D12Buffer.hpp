#pragma once

#include "Fusion/Renderer/Buffer.hpp"

#include "D3D12Common.hpp"

namespace Fusion {

	class D3D12Buffer : public Buffer
	{
	public:
		D3D12Buffer(const BufferInfo& InCreateInfo);
		~D3D12Buffer();

		virtual Byte* Map() override;
		virtual void Unmap(Byte* InPtr) override;

		virtual void Transition(CommandList* InCmdList, EBufferState InState) override;

		virtual void SetData(CommandList* InCmdList, const void* InData, const Shared<Buffer>& InUploadBuffer) override;

		virtual const BufferInfo& GetInfo() const override { return m_CreateInfo; }

		virtual uint64_t GetSize() const override { return m_CreateInfo.Size; }

		auto GetGPUBufferLocation() { return m_Buffer->GetGPUVirtualAddress(); }

		auto& GetBuffer() { return m_Buffer; }
		const auto& GetBuffer() const { return m_Buffer; }

	private:
		BufferInfo m_CreateInfo;
		D3DComPtr<ID3D12Resource2> m_Buffer;
		Shared<D3D12Buffer> m_UploadBuffer = nullptr;
	};

}
