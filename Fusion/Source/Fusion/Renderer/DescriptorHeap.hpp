#pragma once

#include "Fusion/Core/Core.hpp"
#include "RenderTexture.hpp"

namespace Fusion {

	enum class EDescriptorHeapType
	{
		RenderTargetView, SRV_CBV_UAV, Sampler, DepthStencilView
	};

	struct DescriptorHeapInfo
	{
		EDescriptorHeapType Type;
		uint32_t Capacity;
		bool ShaderVisible;
	};

	enum class EAllocationType { RenderTargetView, ShaderResourceView, ConstantBufferView, UnorderedAccessView, DepthStencilView, Sampler };

	class DescriptorHeap;
	struct DescriptorHeapAllocation
	{
		DescriptorHeap* Heap;
		uint32_t Index;
		EAllocationType Type;
	};

	class DescriptorHeap
	{
	public:
		virtual DescriptorHeapAllocation AllocateShaderResourceView(const Shared<RenderTexture>& InRenderTexture, uint32_t InAttachmentIndex, uint32_t InFrameIdx) = 0;
		virtual std::vector<DescriptorHeapAllocation> AllocateShaderResourceViews(const Shared<RenderTexture>& InRenderTexture, uint32_t InAttachmentIndex) = 0;

		virtual uintptr_t GetCPUDescriptorHandle(const DescriptorHeapAllocation& InAlloc) const = 0;
		virtual uintptr_t GetGPUDescriptorHandle(const DescriptorHeapAllocation& InAlloc) const = 0;

		virtual void Deallocate(const std::vector<DescriptorHeapAllocation>& InAllocations) = 0;
		virtual void Deallocate(const DescriptorHeapAllocation& InAlloc) = 0;

		virtual DescriptorHeapAllocation Reserve() = 0;

	public:
		static Unique<DescriptorHeap> Create(const DescriptorHeapInfo& InCreateInfo);
	};

}
