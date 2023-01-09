#include "ImGuiRenderTextureImage.hpp"

#include <Fusion/Renderer/RenderSettings.hpp>

#include "Platform/Vulkan/ImGuiRenderTextureImageVulkan.hpp"

namespace FusionEditor {

	Fusion::Unique<ImGuiRenderTextureImage> ImGuiRenderTextureImage::Create(const std::vector<Fusion::Shared<Fusion::RenderTexture>>& InRenderTextures)
	{
		switch (Fusion::RenderSettings::Get().API)
		{
		case Fusion::ERendererAPI::Vulkan: return Fusion::MakeUnique<ImGuiRenderTextureImageVulkan>(InRenderTextures);
		}

		return nullptr;
	}

}
