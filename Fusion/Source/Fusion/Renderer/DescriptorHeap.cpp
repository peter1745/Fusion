#include "FusionPCH.hpp"
#include "DescriptorHeap.hpp"
#include "Renderer.hpp"

#include "Platform/D3D11/D3D11DescriptorHeap.hpp"
#include "Platform/D3D12/D3D12DescriptorHeap.hpp"

namespace Fusion {

	Unique<DescriptorHeap> DescriptorHeap::Create(const DescriptorHeapInfo& InCreateInfo)
	{
		switch (Renderer::CurrentAPI())
		{
		case ERendererAPI::None: return nullptr;
		case ERendererAPI::D3D11: return MakeUnique<D3D11DescriptorHeap>(InCreateInfo);
		case ERendererAPI::D3D12: return MakeUnique<D3D12DescriptorHeap>(InCreateInfo);
		}

		return nullptr;
	}

}
