#pragma once

#include "UI/ImGuiRenderTextureImage.hpp"

namespace FusionEditor {

	class ImGuiRenderTextureImageD3D11 : public ImGuiRenderTextureImage
	{
	public:
		ImGuiRenderTextureImageD3D11(const Fusion::Shared<Fusion::RenderTexture>& InRenderTexture);

		void DrawImage(const ImVec2& InSize) const override;
		void Resize(uint32_t InWidth, uint32_t InHeight) override;

	private:
		Fusion::Shared<Fusion::RenderTexture> m_TargetTexture = nullptr;
	};

}
