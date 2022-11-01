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
		virtual std::vector<DescriptorHeapAllocation> AllocateRenderTextureViews(const Shared<RenderTexture>& InRenderTexture, uint32_t InAttachmentIndex) = 0;

		virtual DescriptorHeapAllocation Reserve() = 0;

	public:
		static Unique<DescriptorHeap> Create(const DescriptorHeapInfo& InCreateInfo);
	};

}
