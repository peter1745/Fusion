#include "ImGuiRenderTextureImage.hpp"

#include <Fusion/Renderer/RenderSettings.hpp>

#ifdef FUSION_PLATFORM_WINDOWS
	#include <Platform/D3D11/ImGuiRenderTextureImageD3D11.hpp>
	#include <Platform/D3D12/ImGuiRenderTextureImageD3D12.hpp>
#endif

#include "Platform/Vulkan/ImGuiRenderTextureImageVulkan.hpp"

namespace FusionEditor {

	Fusion::Unique<ImGuiRenderTextureImage> ImGuiRenderTextureImage::Create(const Fusion::Shared<Fusion::RenderTexture>& InRenderTexture)
	{
		switch (Fusion::RenderSettings::Get().API)
		{
#ifdef FUSION_PLATFORM_WINDOWS
		case Fusion::ERendererAPI::D3D11: return Fusion::MakeUnique<ImGuiRenderTextureImageD3D11>(InRenderTexture);
		case Fusion::ERendererAPI::D3D12: return Fusion::MakeUnique<ImGuiRenderTextureImageD3D12>(InRenderTexture);
#endif
		case Fusion::ERendererAPI::Vulkan: return Fusion::MakeUnique<ImGuiRenderTextureImageVulkan>(InRenderTexture);
		}

		return nullptr;
	}

}
