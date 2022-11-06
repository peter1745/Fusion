#include "FusionPCH.hpp"
#include "PipelineLayout.hpp"
#include "Renderer.hpp"

#ifdef FUSION_PLATFORM_WINDOWS
	#include <Platform/D3D11/D3D11PipelineLayout.hpp>
	#include <Platform/D3D12/D3D12PipelineLayout.hpp>
#endif

namespace Fusion {

	Unique<PipelineLayout> PipelineLayout::Create(const PipelineLayoutInfo& InCreateInfo)
	{
		switch (Renderer::CurrentAPI())
		{
		case ERendererAPI::None: return nullptr;
		case ERendererAPI::D3D11: return MakeUnique<D3D11PipelineLayout>(InCreateInfo);
		case ERendererAPI::D3D12: return MakeUnique<D3D12PipelineLayout>(InCreateInfo);
		}

		return nullptr;
	}

}
