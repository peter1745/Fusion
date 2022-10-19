#include "EditorViewportWindow.h"

namespace FusionEditor {

	EditorViewportWindow::EditorViewportWindow(const Fusion::Shared<Fusion::World>& InWorld)
		: ViewportWindowBase("MainViewport", InWorld)
	{
		SetTitle("Viewport");

		m_Camera = ViewportCamera(m_ViewportWidth, m_ViewportHeight);
	}

	void EditorViewportWindow::OnUpdate(float InDeltaTime)
	{
		ViewportWindowBase::OnUpdate(InDeltaTime);
		m_Camera.OnUpdate(InDeltaTime);
	}

	void EditorViewportWindow::RenderWorld()
	{
		m_WorldRenderer->Begin(m_Camera, m_Camera.GetViewMatrix());
		m_WorldRenderer->Render();
		m_WorldRenderer->End();
	}

	void EditorViewportWindow::OnResize(uint32_t InWidth, uint32_t InHeight)
	{
		if (InWidth == 0 || InHeight == 0)
			return;

		m_Camera.SetViewportSize(InWidth, InHeight);
	}

}
