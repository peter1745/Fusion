#include "FusionPCH.hpp"
#include "SwapChain.hpp"
#include "RenderSettings.hpp"

#include "Platform/Vulkan/VulkanSwapChain.hpp"

namespace Fusion {

	Shared<SwapChain> SwapChain::Create(const Shared<GraphicsContext>& InContext, const SwapChainInfo& InCreateInfo)
	{
		switch (RenderSettings::Get().API)
		{
		case ERendererAPI::None: return nullptr;
		case ERendererAPI::Vulkan: return Shared<VulkanSwapChain>::Create(InContext, InCreateInfo);
		}

		return nullptr;
	}

}
