#pragma once

#include "Fusion/Renderer/DescriptorHeap.hpp"

#include "D3D12Common.hpp"

namespace Fusion {

	class D3D12DescriptorHeap : public DescriptorHeap
	{
	public:
		D3D12DescriptorHeap(const DescriptorHeapInfo& InCreateInfo);

		virtual std::vector<DescriptorHeapAllocation> AllocateRenderTextureViews(const Shared<RenderTexture>& InRenderTexture, uint32_t InAttachmentIndex) override;

		virtual DescriptorHeapAllocation Reserve() override;

		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandleStart() const { return m_CPUStart; }
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandleStart() const { return m_GPUStart; }

		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t InIndex) const;
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t InIndex) const;

		auto& GetHeap() { return m_DescriptorHeap; }
		const auto& GetHeap() const { return m_DescriptorHeap; }

	private:
		uint32_t FindFreeIndex() const;

	private:
		DescriptorHeapInfo m_CreateInfo;

		D3D12ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap;
		D3D12_CPU_DESCRIPTOR_HANDLE m_CPUStart{};
		D3D12_GPU_DESCRIPTOR_HANDLE m_GPUStart{};
		uint32_t m_HeapIncrementSize = 0;

		uint64_t m_SearchStart = 0;
		std::vector<uint64_t> m_AllocationMap;

		uint32_t m_Count = 0;
	};

}
