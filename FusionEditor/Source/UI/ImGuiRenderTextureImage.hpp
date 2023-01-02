#pragma once

#include <Fusion/Renderer/RenderTexture.hpp>

#include <ImGui/imgui.h>

namespace FusionEditor {

	class ImGuiRenderTextureImage
	{
	public:
		virtual ~ImGuiRenderTextureImage() = default;

		virtual void DrawImage(const ImVec2& InSize) = 0;

		virtual void Resize(uint32_t InWidth, uint32_t InHeight) = 0;

	public:
		static Fusion::Unique<ImGuiRenderTextureImage> Create(const std::vector<Fusion::Shared<Fusion::RenderTexture>>& InRenderTextures);
	};

}
