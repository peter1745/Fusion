#include "FusionPCH.hpp"
#include "D3D11DescriptorHeap.hpp"
#include "D3D11Context.hpp"
#include "D3D11RenderTexture.hpp"

namespace Fusion {

	D3D11DescriptorHeap::D3D11DescriptorHeap(const DescriptorHeapInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo)
	{
	}

	DescriptorHeapAllocation D3D11DescriptorHeap::AllocateShaderResourceView(const Shared<Texture2D>& InTexture)
	{
		return { this, 0 };
	}

	DescriptorHeapAllocation D3D11DescriptorHeap::AllocateShaderResourceView(const Shared<RenderTexture>& InRenderTexture, uint32_t InAttachmentIndex, uint32_t InFrameIdx)
	{
		auto& Device = GraphicsContext::Get<D3D11Context>()->GetDevice().As<D3D11Device>()->GetDevice();

		auto D3DRenderTexture = InRenderTexture.As<D3D11RenderTexture>();
		const auto& RTInfo = D3DRenderTexture->GetInfo();

		Shared<D3D11Image2D> Image = D3DRenderTexture->GetImage(InAttachmentIndex, 0).As<D3D11Image2D>();

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = EFormatToDXGIFormat(RTInfo.ColorAttachments[InAttachmentIndex].Format);
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;
		SRVDesc.Texture2D.MostDetailedMip = 0;
		Device->CreateShaderResourceView(Image->GetResource(), &SRVDesc, m_ShaderResourceViews[m_NextSRVIndex]);

		DescriptorHeapAllocation Allocation = { this, m_NextSRVIndex, EAllocationType::ShaderResourceView };

		m_NextSRVIndex++;
		return { Allocation };
	}

	std::vector<DescriptorHeapAllocation> D3D11DescriptorHeap::AllocateShaderResourceViews(const Shared<RenderTexture>& InRenderTexture, uint32_t InAttachmentIndex)
	{
		return { AllocateShaderResourceView(InRenderTexture, InAttachmentIndex, 0) };
	}

	uintptr_t D3D11DescriptorHeap::GetCPUDescriptorHandle(const DescriptorHeapAllocation& InAlloc) const
	{
		switch (InAlloc.Type)
		{
		case EAllocationType::RenderTargetView:
			break;
		case EAllocationType::ShaderResourceView: return reinterpret_cast<uintptr_t>(m_ShaderResourceViews.at(InAlloc.Index).Get());
		case EAllocationType::ConstantBufferView:
		case EAllocationType::UnorderedAccessView:
		case EAllocationType::DepthStencilView:
		case EAllocationType::Sampler:
			break;
		}

		return reinterpret_cast<uintptr_t>(nullptr);
	}

	uintptr_t D3D11DescriptorHeap::GetGPUDescriptorHandle(const DescriptorHeapAllocation& InAlloc) const
	{
		switch (InAlloc.Type)
		{
		case EAllocationType::RenderTargetView:
			break;
		case EAllocationType::ShaderResourceView: return reinterpret_cast<uintptr_t>(m_ShaderResourceViews.at(InAlloc.Index).Get());
		case EAllocationType::ConstantBufferView:
		case EAllocationType::UnorderedAccessView:
		case EAllocationType::DepthStencilView:
		case EAllocationType::Sampler:
			break;
		}

		return reinterpret_cast<uintptr_t>(nullptr);
	}

	void D3D11DescriptorHeap::Deallocate(const std::vector<DescriptorHeapAllocation>& InAllocations)
	{
		for (const auto& Alloc : InAllocations)
		{
			switch (Alloc.Type)
			{
			case EAllocationType::RenderTargetView:
				break;
			case EAllocationType::ShaderResourceView:
			{
				FUSION_CORE_VERIFY(m_ShaderResourceViews.find(Alloc.Index) != m_ShaderResourceViews.end());
				m_ShaderResourceViews.erase(Alloc.Index);
				break;
			}
			case EAllocationType::ConstantBufferView:
				break;
			case EAllocationType::UnorderedAccessView:
				break;
			case EAllocationType::DepthStencilView:
				break;
			case EAllocationType::Sampler:
				break;
			}
		}
	}

	void D3D11DescriptorHeap::Deallocate(const DescriptorHeapAllocation& InAlloc)
	{
		switch (InAlloc.Type)
		{
		case EAllocationType::RenderTargetView:
			break;
		case EAllocationType::ShaderResourceView:
		{
			FUSION_CORE_VERIFY(m_ShaderResourceViews.find(InAlloc.Index) != m_ShaderResourceViews.end());
			m_ShaderResourceViews.erase(InAlloc.Index);
			break;
		}
		case EAllocationType::ConstantBufferView:
			break;
		case EAllocationType::UnorderedAccessView:
			break;
		case EAllocationType::DepthStencilView:
			break;
		case EAllocationType::Sampler:
			break;
		}
	}

	DescriptorHeapAllocation D3D11DescriptorHeap::Reserve()
	{
		return { this, 0 };
	}

}
