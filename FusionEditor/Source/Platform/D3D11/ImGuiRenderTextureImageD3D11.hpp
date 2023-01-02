#pragma once

#include "UI/ImGuiRenderTextureImage.hpp"

namespace FusionEditor {

	class ImGuiRenderTextureImageD3D11 : public ImGuiRenderTextureImage
	{
	public:
		ImGuiRenderTextureImageD3D11(const std::vector<Fusion::Shared<Fusion::RenderTexture>>& InRenderTextures);

		void DrawImage(const ImVec2& InSize) override;
		void Resize(uint32_t InWidth, uint32_t InHeight) override;

	private:
		std::vector<Fusion::Shared<Fusion::RenderTexture>> m_TargetTextures;
	};

}
