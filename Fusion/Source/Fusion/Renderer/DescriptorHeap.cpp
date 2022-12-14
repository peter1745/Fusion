#include "FusionPCH.hpp"
#include "DescriptorHeap.hpp"
#include "RenderSettings.hpp"

#include "Platform/D3D11/D3D11DescriptorHeap.hpp"
#include "Platform/D3D12/D3D12DescriptorHeap.hpp"

namespace Fusion {

	Shared<DescriptorHeap> DescriptorHeap::Create(const DescriptorHeapInfo& InCreateInfo)
	{
		switch (RenderSettings::Get().API)
		{
		case ERendererAPI::None: return nullptr;
		case ERendererAPI::D3D11: return Shared<D3D11DescriptorHeap>::Create(InCreateInfo);
		case ERendererAPI::D3D12: return Shared<D3D12DescriptorHeap>::Create(InCreateInfo);
		}

		return nullptr;
	}

}
