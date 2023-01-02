#include "FusionPCH.hpp"
#include "RenderTexture.hpp"
#include "RenderSettings.hpp"

#include "Platform/Vulkan/VulkanRenderTexture.hpp"

#ifdef FUSION_PLATFORM_WINDOWS
	#include "Platform/D3D11/D3D11RenderTexture.hpp"
	#include "Platform/D3D12/D3D12RenderTexture.hpp"
#endif

namespace Fusion {

	Shared<RenderTexture> RenderTexture::Create(const RenderTextureInfo& InCreateInfo)
	{
		switch (RenderSettings::Get().API)
		{
		case ERendererAPI::None: return nullptr;
#ifdef FUSION_PLATFORM_WINDOWS
		case ERendererAPI::D3D11: return Shared<D3D11RenderTexture>::Create(InCreateInfo);
		case ERendererAPI::D3D12: return Shared<D3D12RenderTexture>::Create(InCreateInfo);
#endif
		case ERendererAPI::Vulkan: return Shared<VulkanRenderTexture>::Create(InCreateInfo);
		}

		return nullptr;
	}

}
