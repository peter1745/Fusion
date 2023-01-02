#pragma once

#include "Fusion/Renderer/DescriptorHeap.hpp"

#include "D3D12Device.hpp"
#include "D3D12Buffer.hpp"

namespace Fusion {

	class D3D12DescriptorHeap : public DescriptorHeap
	{
	public:
		D3D12DescriptorHeap(const Shared<D3D12Device>& InDevice, const DescriptorHeapInfo& InCreateInfo);
		D3D12DescriptorHeap(const DescriptorHeapInfo& InCreateInfo);

		virtual DescriptorHeapAllocation AllocateShaderResourceView(const Shared<Texture2D>& InTexture) override;
		virtual DescriptorHeapAllocation AllocateShaderResourceView(const Shared<RenderTexture>& InRenderTexture, uint32_t InAttachmentIndex) override;

		DescriptorHeapAllocation AllocateConstantBufferView(D3D12Buffer* InBuffer, uint32_t InSize);

		virtual void Deallocate(const std::vector<DescriptorHeapAllocation>& InAllocations) override;
		virtual void Deallocate(const DescriptorHeapAllocation& InAlloc) override;

		virtual DescriptorHeapAllocation Reserve() override;

		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandleStart() const { return m_CPUStart; }
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandleStart() const { return m_GPUStart; }

		uintptr_t GetCPUDescriptorHandle(const DescriptorHeapAllocation& InAlloc) const;
		uintptr_t GetGPUDescriptorHandle(const DescriptorHeapAllocation& InAlloc) const;

		auto& GetHeap() { return m_DescriptorHeap; }
		const auto& GetHeap() const { return m_DescriptorHeap; }

	private:
		uint32_t FindFreeIndex() const;

	private:
		DescriptorHeapInfo m_CreateInfo;

		Shared<D3D12Device> m_Device;

		D3DComPtr<ID3D12DescriptorHeap> m_DescriptorHeap;
		D3D12_CPU_DESCRIPTOR_HANDLE m_CPUStart{};
		D3D12_GPU_DESCRIPTOR_HANDLE m_GPUStart{};
		uint32_t m_HeapIncrementSize = 0;

		uint64_t m_SearchStart = 0;
		std::vector<uint64_t> m_AllocationMap;

		uint32_t m_Count = 0;
	};

}
