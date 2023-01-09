#include "FusionPCH.hpp"
#include "GraphicsPipeline.hpp"
#include "RenderSettings.hpp"

#include "Platform/Vulkan/VulkanGraphicsPipeline.hpp"

namespace Fusion {

	Unique<GraphicsPipeline> GraphicsPipeline::Create(const GraphicsPipelineInfo& InCreateInfo)
	{
		switch (RenderSettings::Get().API)
		{
		case ERendererAPI::None: return nullptr;
		case ERendererAPI::Vulkan: return MakeUnique<VulkanGraphicsPipeline>(InCreateInfo);
		}

		return nullptr;
	}

}
