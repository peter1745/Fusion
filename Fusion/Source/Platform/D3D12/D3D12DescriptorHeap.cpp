#include "FusionPCH.hpp"
#include "D3D12DescriptorHeap.hpp"
#include "D3D12Context.hpp"
#include "D3D12RenderTexture.hpp"
#include "D3D12Texture.hpp"

#include "Fusion/Memory/Utils.h"

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

	D3D12DescriptorHeap::D3D12DescriptorHeap(const Shared<D3D12Device>& InDevice, const DescriptorHeapInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo), m_Device(InDevice), m_AllocationMap(InCreateInfo.Capacity >> 6, ~0ULL)
	{
		D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc = {};
		DescriptorHeapDesc.Type = EDescriptorHeapTypeToD3D12DescriptorHeapType(InCreateInfo.Type);
		DescriptorHeapDesc.NumDescriptors = InCreateInfo.Capacity;
		DescriptorHeapDesc.Flags = InCreateInfo.ShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		DescriptorHeapDesc.NodeMask = 0;
		m_Device->GetDevice()->CreateDescriptorHeap(&DescriptorHeapDesc, m_DescriptorHeap, m_DescriptorHeap);

		m_CPUStart = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		if (InCreateInfo.ShaderVisible)
			m_GPUStart = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();

		m_HeapIncrementSize = m_Device->GetDevice()->GetDescriptorHandleIncrementSize(DescriptorHeapDesc.Type);
	}

	D3D12DescriptorHeap::D3D12DescriptorHeap(const DescriptorHeapInfo& InCreateInfo)
	    : m_CreateInfo(InCreateInfo), m_Device(GraphicsContext::Get<D3D12Context>()->GetDevice()), m_AllocationMap(InCreateInfo.Capacity >> 6, ~0ULL)
	{
		D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc = {};
		DescriptorHeapDesc.Type = EDescriptorHeapTypeToD3D12DescriptorHeapType(InCreateInfo.Type);
		DescriptorHeapDesc.NumDescriptors = InCreateInfo.Capacity;
		DescriptorHeapDesc.Flags = InCreateInfo.ShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		DescriptorHeapDesc.NodeMask = 0;
		m_Device->GetDevice()->CreateDescriptorHeap(&DescriptorHeapDesc, m_DescriptorHeap, m_DescriptorHeap);

		m_CPUStart = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		if (InCreateInfo.ShaderVisible)
			m_GPUStart = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();

		m_HeapIncrementSize = m_Device->GetDevice()->GetDescriptorHandleIncrementSize(DescriptorHeapDesc.Type);
	}

	DescriptorHeapAllocation D3D12DescriptorHeap::AllocateShaderResourceView(const Shared<Texture2D>& InTexture)
	{
		auto D3DTexture = InTexture.As<D3D12Texture2D>();
		const auto& TextureInfo = InTexture->GetInfo();

		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = EFormatToDXGIFormat(TextureInfo.Format);
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRVDesc.Texture2D.MipLevels = 1;
		SRVDesc.Texture2D.MostDetailedMip = 0;

		auto Allocation = Reserve();
		if (Allocation.Heap == nullptr)
			return { nullptr, ~0U };

		Allocation.Type = EAllocationType::ShaderResourceView;

		D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle = m_CPUStart;
		DescriptorHandle.ptr += Allocation.Index * m_HeapIncrementSize;
		m_Device->GetDevice()->CreateShaderResourceView(D3DTexture->GetImage().As<D3D12Image2D>()->GetResource(), &SRVDesc, DescriptorHandle);

		return Allocation;
	}

	DescriptorHeapAllocation D3D12DescriptorHeap::AllocateShaderResourceView(const Shared<RenderTexture>& InRenderTexture, uint32_t InAttachmentIndex, uint32_t InFrameIdx)
	{
		auto D3DRenderTexture = InRenderTexture.As<D3D12RenderTexture>();
		const auto& RTInfo = D3DRenderTexture->GetInfo();

		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = EFormatToDXGIFormat(RTInfo.ColorAttachments[InAttachmentIndex].Format);
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRVDesc.Texture2D.MipLevels = 1;
		SRVDesc.Texture2D.MostDetailedMip = 0;
		SRVDesc.Texture2D.PlaneSlice = 0;
		SRVDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		auto& Attachment = D3DRenderTexture->GetColorAttachments()[InAttachmentIndex];

		auto Allocation = Reserve();
		if (Allocation.Heap == nullptr)
			return { nullptr, ~0U };

		Allocation.Type = EAllocationType::ShaderResourceView;

		D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle = m_CPUStart;
		DescriptorHandle.ptr += Allocation.Index * m_HeapIncrementSize;

		m_Device->GetDevice()->CreateShaderResourceView(Attachment.Images[InFrameIdx]->GetResource(), &SRVDesc, DescriptorHandle);

		return Allocation;
	}

	std::vector<DescriptorHeapAllocation> D3D12DescriptorHeap::AllocateShaderResourceViews(const Shared<RenderTexture>& InRenderTexture, uint32_t InAttachmentIndex)
	{
		auto D3DRenderTexture = InRenderTexture.As<D3D12RenderTexture>();
		const auto& RTInfo = D3DRenderTexture->GetInfo();

		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = EFormatToDXGIFormat(RTInfo.ColorAttachments[InAttachmentIndex].Format);
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
			auto Allocation = Reserve();
			if (Allocation.Heap == nullptr)
				break;

			Allocation.Type = EAllocationType::ShaderResourceView;

			D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle = m_CPUStart;
			DescriptorHandle.ptr += Allocation.Index * m_HeapIncrementSize;

			m_Device->GetDevice()->CreateShaderResourceView(Attachment.Images[Idx]->GetResource(), &SRVDesc, DescriptorHandle);

			Allocations[Idx] = Allocation;
		}

		return Allocations;
	}

	DescriptorHeapAllocation D3D12DescriptorHeap::AllocateConstantBufferView(D3D12Buffer* InBuffer, uint32_t InSize)
	{
		auto Allocation = Reserve();
		if (Allocation.Heap == nullptr)
			return { nullptr, ~0U };

		Allocation.Type = EAllocationType::ConstantBufferView;

		D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle = m_CPUStart;
		DescriptorHandle.ptr += Allocation.Index * m_HeapIncrementSize;

		D3D12_CONSTANT_BUFFER_VIEW_DESC ViewDesc = {};
		ViewDesc.BufferLocation = InBuffer->GetGPUBufferLocation();
		ViewDesc.SizeInBytes = Align<256>(InSize);

		m_Device->GetDevice()->CreateConstantBufferView(&ViewDesc, DescriptorHandle);

		return Allocation;
	}

	void D3D12DescriptorHeap::Deallocate(const DescriptorHeapAllocation& InAlloc)
	{
		m_SearchStart = InAlloc.Index >> 6;
		m_AllocationMap[m_SearchStart] |= 1ULL << (InAlloc.Index & 0x3F);
		m_Count--;
	}

	void D3D12DescriptorHeap::Deallocate(const std::vector<DescriptorHeapAllocation>& InAllocations)
	{
		for (const auto& Allocation : InAllocations)
			Deallocate(Allocation);
	}

	DescriptorHeapAllocation D3D12DescriptorHeap::Reserve()
	{
		uint32_t FreeHeapIdx = FindFreeIndex();

		if (FreeHeapIdx == ~0U)
			return { nullptr, ~0U };

		m_SearchStart = FreeHeapIdx >> 6;

		D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle = m_CPUStart;
		DescriptorHandle.ptr += FreeHeapIdx * m_HeapIncrementSize;

		m_AllocationMap[m_SearchStart] &= ~(1ULL << (FreeHeapIdx & 0x3F));
		m_Count++;

		return { this, FreeHeapIdx };
	}

	uintptr_t D3D12DescriptorHeap::GetCPUDescriptorHandle(const DescriptorHeapAllocation& InAlloc) const
	{
		D3D12_CPU_DESCRIPTOR_HANDLE Handle = m_CPUStart;
		Handle.ptr += InAlloc.Index * m_HeapIncrementSize;
		return Handle.ptr;
	}

	uintptr_t D3D12DescriptorHeap::GetGPUDescriptorHandle(const DescriptorHeapAllocation& InAlloc) const
	{
		FUSION_CORE_VERIFY(m_CreateInfo.ShaderVisible, "Can't get GPU handle from a heap that isn't shader visibile!")
		D3D12_GPU_DESCRIPTOR_HANDLE Handle = m_GPUStart;
		Handle.ptr += InAlloc.Index * m_HeapIncrementSize;
		return Handle.ptr;
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
