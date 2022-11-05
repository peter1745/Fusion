#pragma once

#include "Fusion/Core/Core.hpp"
#include "Fusion/Renderer/UniformBuffer.hpp"

#include "D3D12Common.hpp"
#include "D3D12Buffer.hpp"
#include "D3D12DescriptorHeap.hpp"

namespace Fusion {

	class D3D12UniformBuffer : public UniformBuffer
	{
	public:
		D3D12UniformBuffer(DescriptorHeap* InHeap, const ConstantBufferInfo& InCreateInfo);
		~D3D12UniformBuffer();

		virtual void SetData(void* InData) override;

		virtual uint32_t GetSize() const override;
		virtual EShaderBindPoint GetBindPoint() const override;

		const Unique<D3D12Buffer>& GetBuffer() const { return m_Buffer; }

	private:
		Unique<D3D12Buffer> m_Buffer;
		DescriptorHeap* m_Heap = nullptr;
		DescriptorHeapAllocation m_Allocation{};
	};
}
