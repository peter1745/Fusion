#include "EditorViewportWindow.h"

namespace FusionEditor {

	EditorViewportWindow::EditorViewportWindow(Fusion::World* InWorld)
		: EditorWindow("MainViewport", 500, 300), m_ViewportWidth(500.0f), m_ViewportHeight(300.0f)
	{
		SetTitle("Viewport");

		m_Camera = ViewportCamera(m_ViewportWidth, m_ViewportHeight);
		m_WorldRenderer = Fusion::MakeUnique<Fusion::WorldRenderer>(InWorld, uint32_t(m_ViewportWidth), uint32_t(m_ViewportHeight));
	}

	void EditorViewportWindow::OnRender()
	{
		m_WorldRenderer->Begin(m_Camera, m_Camera.GetViewMatrix());
		m_WorldRenderer->Render();
		m_WorldRenderer->End();
	}

	void EditorViewportWindow::OnUpdate(float InDeltaTime)
	{
		if (m_ViewportWidth != GetWindowWidth() || m_ViewportHeight != GetWindowHeight())
		{
			m_ViewportWidth = GetWindowWidth();
			m_ViewportHeight = GetWindowHeight();

			m_Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
			m_WorldRenderer->SetViewportSize(m_ViewportWidth, m_ViewportHeight);
		}

		m_Camera.OnUpdate(InDeltaTime);
	}

	void EditorViewportWindow::RenderContents()
	{
		auto FinalImage = m_WorldRenderer->GetFinalImage();
		ImTextureID ColorAttachmentImageID = reinterpret_cast<ImTextureID>(FinalImage->GetColorAttachmentID(0));
		ImGui::Image(ColorAttachmentImageID, ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
	}

}
