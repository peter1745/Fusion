#include "ViewportWindow.h"

namespace FusionEditor {

	ViewportWindow::ViewportWindow(const Fusion::Shared<Fusion::Framebuffer>& InFramebuffer)
		: EditorWindow("MainViewport", 500, 300), m_Framebuffer(InFramebuffer)
	{
		SetTitle("Viewport");
	}

	void ViewportWindow::RenderContents()
	{
		ImTextureID ColorAttachmentImageID = reinterpret_cast<ImTextureID>(m_Framebuffer->GetColorAttachmentID(0));
		ImGui::Image(ColorAttachmentImageID, ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
	}

}
