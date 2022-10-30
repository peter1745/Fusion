#include "FusionPCH.hpp"
#include "SwapChain.hpp"
#include "Renderer.hpp"

#include "Platform/D3D11/D3D11SwapChain.hpp"

namespace Fusion {

	Shared<SwapChain> SwapChain::Create(const SwapChainInfo& InCreateInfo)
	{
		switch (Renderer::CurrentAPI())
		{
		case ERendererAPI::None: return nullptr;
		case ERendererAPI::D3D11: return Shared<D3D11SwapChain>::Create(InCreateInfo);
		}

		return nullptr;
	}

}
