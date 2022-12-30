#include "ImGuiRenderTextureImageD3D11.hpp"

namespace FusionEditor {

	ImGuiRenderTextureImageD3D11::ImGuiRenderTextureImageD3D11(const Fusion::Shared<Fusion::RenderTexture>& InRenderTexture)
	    : m_TargetTexture(InRenderTexture)
	{
	}

	void ImGuiRenderTextureImageD3D11::DrawImage(const ImVec2& InSize) const
	{
		ImTextureID TexID = static_cast<ImTextureID>(m_TargetTexture->GetColorTextureID(0));
		ImGui::Image(TexID, InSize);
	}

	void ImGuiRenderTextureImageD3D11::Resize(uint32_t InWidth, uint32_t InHeight)
	{
		// TODO(Peter): Do this in a loop to make this generic
		m_TargetTexture->Resize(0, 0, { InWidth, InHeight });
		m_TargetTexture->Resize(1, 0, { InWidth, InHeight });
	}

}
