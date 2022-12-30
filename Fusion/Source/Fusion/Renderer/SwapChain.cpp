#include "FusionPCH.hpp"
#include "SwapChain.hpp"
#include "RenderSettings.hpp"

#ifdef FUSION_PLATFORM_WINDOWS
	#include "Platform/D3D11/D3D11SwapChain.hpp"
	#include "Platform/D3D12/D3D12SwapChain.hpp"
#endif

#include "Platform/Vulkan/VulkanSwapChain.hpp"

namespace Fusion {

	Shared<SwapChain> SwapChain::Create(const Shared<GraphicsContext>& InContext, const SwapChainInfo& InCreateInfo)
	{
		switch (RenderSettings::Get().API)
		{
		case ERendererAPI::None: return nullptr;
#ifdef FUSION_PLATFORM_WINDOWS
		case ERendererAPI::D3D11: return Shared<D3D11SwapChain>::Create(InContext, InCreateInfo);
		case ERendererAPI::D3D12: return Shared<D3D12SwapChain>::Create(InContext, InCreateInfo);
#endif
		case ERendererAPI::Vulkan: return Shared<VulkanSwapChain>::Create(InContext, InCreateInfo);
		}

		return nullptr;
	}

}
