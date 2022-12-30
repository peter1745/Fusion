#include "FusionPCH.hpp"
#include "Renderer.hpp"
#include "RenderSettings.hpp"

#include "Platform/Vulkan/VulkanRenderer.hpp"

#ifdef FUSION_PLATFORM_WINDOWS
	#include "Platform/D3D11/D3D11Renderer.hpp"
	#include "Platform/D3D12/D3D12Renderer.hpp"
#endif

namespace Fusion {

	static Renderer* s_Instance = nullptr;

	Unique<Renderer> Renderer::Create(const RendererInfo& InInfo)
	{
		Renderer* Result = nullptr;

		switch (RenderSettings::Get().API)
		{
		case ERendererAPI::None:
			Result = nullptr;
			break;
#ifdef FUSION_PLATFORM_WINDOWS
		case ERendererAPI::D3D11:
			Result = new D3D11Renderer(InInfo);
			break;
		case ERendererAPI::D3D12:
			Result = new D3D12Renderer(InInfo);
			break;
#endif
		case ERendererAPI::Vulkan:
			Result = new VulkanRenderer(InInfo);
			break;
		}

		s_Instance = Result;
		return Unique<Renderer>(Result);
	}

	Renderer& Renderer::GetCurrent()
	{
		return *s_Instance;
	}

}