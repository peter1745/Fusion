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

	class DescriptorHeap;
	struct DescriptorHeapAllocation
	{
		DescriptorHeap* Heap;
		uint32_t Index;
	};

	class DescriptorHeap
	{
	public:
		virtual DescriptorHeapAllocation AllocateRenderTextureView(const Shared<RenderTexture>& InRenderTexture, uint32_t InAttachmentIndex, uint32_t InFrameIdx) = 0;
		virtual std::vector<DescriptorHeapAllocation> AllocateRenderTextureViews(const Shared<RenderTexture>& InRenderTexture, uint32_t InAttachmentIndex) = 0;
		virtual void Deallocate(const std::vector<DescriptorHeapAllocation>& InAllocations) = 0;
		virtual void Deallocate(uint32_t InAllocIndex) = 0;

		virtual DescriptorHeapAllocation Reserve() = 0;

	public:
		static Unique<DescriptorHeap> Create(const DescriptorHeapInfo& InCreateInfo);
	};

}
