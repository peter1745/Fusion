#include "FusionPCH.hpp"
#include "GraphicsPipeline.hpp"
#include "Renderer.hpp"

#ifdef FUSION_PLATFORM_WINDOWS
	#include "Platform/D3D12/D3D12GraphicsPipeline.hpp"
#endif

namespace Fusion {

	Unique<GraphicsPipeline> GraphicsPipeline::Create(const GraphicsPipelineInfo& InCreateInfo)
	{
		switch (Renderer::CurrentAPI())
		{
		case ERendererAPI::None: return nullptr;
		case ERendererAPI::D3D11: return nullptr;
		case ERendererAPI::D3D12: return MakeUnique<D3D12GraphicsPipeline>(InCreateInfo);
		}

		return nullptr;
	}

}
