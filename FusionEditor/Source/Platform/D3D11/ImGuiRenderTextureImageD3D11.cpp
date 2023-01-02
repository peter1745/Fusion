#include "ImGuiRenderTextureImageD3D11.hpp"

#include <Fusion/Renderer/Renderer.hpp>

namespace FusionEditor {

	ImGuiRenderTextureImageD3D11::ImGuiRenderTextureImageD3D11(const std::vector<Fusion::Shared<Fusion::RenderTexture>>& InRenderTextures)
	    : m_TargetTextures(InRenderTextures)
	{
	}

	void ImGuiRenderTextureImageD3D11::DrawImage(const ImVec2& InSize)
	{
		uint32_t FrameIndex = Fusion::Renderer::GetCurrent().GetCurrentFrame();
		ImTextureID TexID = static_cast<ImTextureID>(m_TargetTextures[FrameIndex]->GetColorTextureID(0));
		ImGui::Image(TexID, InSize);
	}

	void ImGuiRenderTextureImageD3D11::Resize(uint32_t InWidth, uint32_t InHeight)
	{
		uint32_t FrameIndex = Fusion::Renderer::GetCurrent().GetCurrentFrame();
		
		// TODO(Peter): Do this in a loop to make this generic
		m_TargetTextures[FrameIndex]->Resize(0, { InWidth, InHeight });
		m_TargetTextures[FrameIndex]->Resize(1, { InWidth, InHeight });
	}

}
