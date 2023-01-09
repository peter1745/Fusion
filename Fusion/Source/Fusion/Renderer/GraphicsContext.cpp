#include "FusionPCH.hpp"
#include "GraphicsContext.hpp"
#include "RenderSettings.hpp"

#include "Platform/Vulkan/VulkanContext.hpp"

namespace Fusion {

	Shared<GraphicsContext> GraphicsContext::Create()
	{
		switch (RenderSettings::Get().API)
		{
		case ERendererAPI::None: return nullptr;
		case ERendererAPI::Vulkan: return Shared<VulkanContext>::Create();
		}

		return nullptr;
	}

}
