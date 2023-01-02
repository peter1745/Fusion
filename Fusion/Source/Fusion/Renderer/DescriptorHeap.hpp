#pragma once

#include "Fusion/Core/Core.hpp"
#include "RenderTexture.hpp"
#include "Texture.hpp"

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
		DescriptorHeap* Heap = nullptr;
		uint32_t Index = 0;
		EAllocationType Type;
	};

	class DescriptorHeap : public SharedObject
	{
	public:
		virtual ~DescriptorHeap() = default;

		virtual DescriptorHeapAllocation AllocateShaderResourceView(const Shared<Texture2D>& InTexture) = 0;
		virtual DescriptorHeapAllocation AllocateShaderResourceView(const Shared<RenderTexture>& InRenderTexture, uint32_t InAttachmentIndex) = 0;

		virtual uintptr_t GetCPUDescriptorHandle(const DescriptorHeapAllocation& InAlloc) const = 0;
		virtual uintptr_t GetGPUDescriptorHandle(const DescriptorHeapAllocation& InAlloc) const = 0;

		virtual void Deallocate(const std::vector<DescriptorHeapAllocation>& InAllocations) = 0;
		virtual void Deallocate(const DescriptorHeapAllocation& InAlloc) = 0;

		virtual DescriptorHeapAllocation Reserve() = 0;

	public:
		static Shared<DescriptorHeap> Create(const DescriptorHeapInfo& InCreateInfo);
	};

}
