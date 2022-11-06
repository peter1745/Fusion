#pragma once

#include "Fusion/Renderer/DescriptorHeap.hpp"

#include "D3D11Common.hpp"

namespace Fusion {

	class D3D11DescriptorHeap : public DescriptorHeap
	{
	public:
		D3D11DescriptorHeap(const DescriptorHeapInfo& InCreateInfo);

		virtual DescriptorHeapAllocation AllocateShaderResourceView(const Shared<RenderTexture>& InRenderTexture, uint32_t InAttachmentIndex, uint32_t InFrameIdx) override;
		virtual std::vector<DescriptorHeapAllocation> AllocateShaderResourceViews(const Shared<RenderTexture>& InRenderTexture, uint32_t InAttachmentIndex) override;

		//DescriptorHeapAllocation AllocateConstantBufferView(D3D11Buffer* InBuffer, uint32_t InSize);

		virtual uintptr_t GetCPUDescriptorHandle(const DescriptorHeapAllocation& InAlloc) const;
		virtual uintptr_t GetGPUDescriptorHandle(const DescriptorHeapAllocation& InAlloc) const;

		virtual void Deallocate(const std::vector<DescriptorHeapAllocation>& InAllocations) override;
		virtual void Deallocate(const DescriptorHeapAllocation& InAlloc) override;

		virtual DescriptorHeapAllocation Reserve() override;

	private:
		DescriptorHeapInfo m_CreateInfo;

		std::unordered_map<uint32_t, D3DComPtr<ID3D11ShaderResourceView>> m_ShaderResourceViews;
		uint32_t m_NextSRVIndex = 0;
	};

}
