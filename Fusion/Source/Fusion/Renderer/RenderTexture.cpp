#include "FusionPCH.hpp"
#include "RenderTexture.hpp"
#include "RenderSettings.hpp"

#include "Platform/Vulkan/VulkanRenderTexture.hpp"

namespace Fusion {

	Shared<RenderTexture> RenderTexture::Create(const RenderTextureInfo& InCreateInfo)
	{
		switch (RenderSettings::Get().API)
		{
		case ERendererAPI::None: return nullptr;
		case ERendererAPI::Vulkan: return Shared<VulkanRenderTexture>::Create(InCreateInfo);
		}

		return nullptr;
	}

}
