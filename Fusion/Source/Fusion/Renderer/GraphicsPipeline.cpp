#include "FusionPCH.hpp"
#include "GraphicsPipeline.hpp"
#include "RenderSettings.hpp"

#ifdef FUSION_PLATFORM_WINDOWS
#include "Platform/D3D11/D3D11GraphicsPipeline.hpp"
#include "Platform/D3D12/D3D12GraphicsPipeline.hpp"
#endif

namespace Fusion {

	Unique<GraphicsPipeline> GraphicsPipeline::Create(const GraphicsPipelineInfo& InCreateInfo)
	{
		switch (RenderSettings::Get().API)
		{
		case ERendererAPI::None: return nullptr;
#ifdef FUSION_PLATFORM_WINDOWS
		case ERendererAPI::D3D11: return MakeUnique<D3D11GraphicsPipeline>(InCreateInfo);
		case ERendererAPI::D3D12: return MakeUnique<D3D12GraphicsPipeline>(InCreateInfo);
#endif
		case ERendererAPI::Vulkan: return nullptr;
		}

		return nullptr;
	}

}
