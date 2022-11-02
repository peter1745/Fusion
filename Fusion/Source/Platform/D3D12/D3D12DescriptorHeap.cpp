#include "FusionPCH.hpp"
#include "D3D12DescriptorHeap.hpp"
#include "D3D12Context.hpp"
#include "D3D12RenderTexture.hpp"

#include <intrin.h>

namespace Fusion {

	static constexpr D3D12_DESCRIPTOR_HEAP_TYPE EDescriptorHeapTypeToD3D12DescriptorHeapType(EDescriptorHeapType InType)
	{
		switch (InType)
		{
		case EDescriptorHeapType::RenderTargetView: return D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		case EDescriptorHeapType::SRV_CBV_UAV: return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		case EDescriptorHeapType::Sampler: return D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		case EDescriptorHeapType::DepthStencilView: return D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		}

		return D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
	}

	D3D12DescriptorHeap::D3D12DescriptorHeap(const DescriptorHeapInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo), m_AllocationMap(InCreateInfo.Capacity >> 6, ~0ULL)
	{
		auto Context = GraphicsContext::Get<Fusion::D3D12Context>();
		auto& Device = Context->GetDevice();

		D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc = {};
		DescriptorHeapDesc.Type = EDescriptorHeapTypeToD3D12DescriptorHeapType(InCreateInfo.Type);
		DescriptorHeapDesc.NumDescriptors = InCreateInfo.Capacity;
		DescriptorHeapDesc.Flags = InCreateInfo.ShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		DescriptorHeapDesc.NodeMask = 0;
		Device->CreateDescriptorHeap(&DescriptorHeapDesc, m_DescriptorHeap, m_DescriptorHeap);

		m_CPUStart = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		m_GPUStart = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		m_HeapIncrementSize = Device->GetDescriptorHandleIncrementSize(DescriptorHeapDesc.Type);
	}

	DescriptorHeapAllocation D3D12DescriptorHeap::AllocateRenderTextureView(const Shared<RenderTexture>& InRenderTexture, uint32_t InAttachmentIndex, uint32_t InFrameIdx)
	{
		auto& Device = GraphicsContext::Get<Fusion::D3D12Context>()->GetDevice();

		auto D3DRenderTexture = InRenderTexture.As<D3D12RenderTexture>();
		const auto& RTInfo = D3DRenderTexture->GetInfo();

		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = ImageFormatToDXGIFormat(RTInfo.ColorAttachments[InAttachmentIndex].Format);
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRVDesc.Texture2D.MipLevels = 1;
		SRVDesc.Texture2D.MostDetailedMip = 0;
		SRVDesc.Texture2D.PlaneSlice = 0;
		SRVDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		auto& Attachment = D3DRenderTexture->GetColorAttachments()[InAttachmentIndex];

		DescriptorHeapAllocation Allocation = {};

		uint32_t FreeHeapIdx = FindFreeIndex();
		if (FreeHeapIdx == ~0U)
			return { nullptr, ~0U };

		m_SearchStart = FreeHeapIdx >> 6;

		D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle = m_CPUStart;
		DescriptorHandle.ptr += FreeHeapIdx * m_HeapIncrementSize;

		Device->CreateShaderResourceView(Attachment.Images[InFrameIdx]->GetResource(), &SRVDesc, DescriptorHandle);

		m_AllocationMap[m_SearchStart] &= ~(1ULL << FreeHeapIdx & 0x3F);
		m_Count++;

		return { this, FreeHeapIdx };
	}

	std::vector<DescriptorHeapAllocation> D3D12DescriptorHeap::AllocateRenderTextureViews(const Shared<RenderTexture>& InRenderTexture, uint32_t InAttachmentIndex)
	{
		auto& Device = GraphicsContext::Get<Fusion::D3D12Context>()->GetDevice();

		auto D3DRenderTexture = InRenderTexture.As<D3D12RenderTexture>();
		const auto& RTInfo = D3DRenderTexture->GetInfo();

		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = ImageFormatToDXGIFormat(RTInfo.ColorAttachments[InAttachmentIndex].Format);
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRVDesc.Texture2D.MipLevels = 1;
		SRVDesc.Texture2D.MostDetailedMip = 0;
		SRVDesc.Texture2D.PlaneSlice = 0;
		SRVDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		auto& Attachment = D3DRenderTexture->GetColorAttachments()[InAttachmentIndex];

		std::vector<DescriptorHeapAllocation> Allocations(Attachment.Images.size());
		for (size_t Idx = 0; Idx < Attachment.Images.size(); Idx++)
		{
			uint32_t FreeHeapIdx = FindFreeIndex();
			if (FreeHeapIdx == ~0U)
				break;

			m_SearchStart = FreeHeapIdx >> 6;

			D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle = m_CPUStart;
			DescriptorHandle.ptr += FreeHeapIdx * m_HeapIncrementSize;

			Device->CreateShaderResourceView(Attachment.Images[Idx]->GetResource(), &SRVDesc, DescriptorHandle);

			m_AllocationMap[m_SearchStart] &= ~(1ULL << FreeHeapIdx & 0x3F);
			m_Count++;

			Allocations[Idx] = { this, FreeHeapIdx };
		}

		return Allocations;
	}

	void D3D12DescriptorHeap::Deallocate(uint32_t InAllocIndex)
	{
		m_SearchStart = InAllocIndex >> 6;
		m_AllocationMap[m_SearchStart] |= 1ULL << (InAllocIndex & 0x3F);
		m_Count--;
	}

	void D3D12DescriptorHeap::Deallocate(const std::vector<DescriptorHeapAllocation>& InAllocations)
	{
		for (const auto& Allocation : InAllocations)
			Deallocate(Allocation.Index);
	}

	Fusion::DescriptorHeapAllocation D3D12DescriptorHeap::Reserve()
	{
		uint32_t FreeHeapIdx = FindFreeIndex();
		m_SearchStart = FreeHeapIdx >> 6;

		D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle = m_CPUStart;
		DescriptorHandle.ptr += FreeHeapIdx * m_HeapIncrementSize;

		m_AllocationMap[m_SearchStart] &= ~(1ULL << FreeHeapIdx & 0x3F);
		m_Count++;

		return { this, FreeHeapIdx };
	}

	D3D12_CPU_DESCRIPTOR_HANDLE D3D12DescriptorHeap::GetCPUDescriptorHandle(uint32_t InIndex) const
	{
		D3D12_CPU_DESCRIPTOR_HANDLE Handle = m_CPUStart;
		Handle.ptr += InIndex * m_HeapIncrementSize;
		return Handle;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE D3D12DescriptorHeap::GetGPUDescriptorHandle(uint32_t InIndex) const
	{
		D3D12_GPU_DESCRIPTOR_HANDLE Handle = m_GPUStart;
		Handle.ptr += InIndex * m_HeapIncrementSize;
		return Handle;
	}

	uint32_t D3D12DescriptorHeap::FindFreeIndex() const
	{
		if (m_Count == m_CreateInfo.Capacity)
			return ~0U;

		for (uint32_t Idx = m_SearchStart; Idx < m_AllocationMap.size(); Idx++)
		{
			unsigned long FreeIndex = 0;
			auto Result = _BitScanForward64(&FreeIndex, m_AllocationMap[Idx]);
			FreeIndex = Result ? FreeIndex : ~0U;

			if (FreeIndex != ~0U)
				return Idx << 6 | static_cast<uint32_t>(FreeIndex);
		}

		return ~0U;
	}

}
