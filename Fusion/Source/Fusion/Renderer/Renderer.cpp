#include "FusionPCH.hpp"
#include "Renderer.hpp"
#include "RenderSettings.hpp"

#include "Platform/Vulkan/VulkanRenderer.hpp"

namespace Fusion {

	Unique<Renderer> Renderer::Create(const RendererInfo& InInfo)
	{
		switch (RenderSettings::Get().API)
		{
		case ERendererAPI::None: return nullptr;
#ifdef FUSION_PLATFORM_WINDOWS
		case ERendererAPI::D3D11: return nullptr;
		case ERendererAPI::D3D12: return nullptr;
#endif
		case ERendererAPI::Vulkan: return MakeUnique<VulkanRenderer>(InInfo);
		}

		return nullptr;
	}

}