#include "EditorViewportWindow.h"

#include "UI/UILibrary.h"

namespace FusionEditor {

	EditorViewportWindow::EditorViewportWindow(const Fusion::Shared<Fusion::World>& InWorld)
		: ViewportWindowBase("MainViewport", InWorld), m_Camera(1280, 720)
	{
		SetTitle("Viewport");
	}

	void EditorViewportWindow::OnUpdate(float InDeltaTime)
	{
		ViewportWindowBase::OnUpdate(InDeltaTime);
		m_Camera.SetActive(m_IsMouseInside && IsTabFocused());
		m_Camera.OnUpdate(InDeltaTime);
	}

	void EditorViewportWindow::RenderContents()
	{
		ImVec2 CursorPos = ImGui::GetCursorPos();
		ImVec2 WindowSize = ImGui::GetWindowSize();
		ImVec2 WindowPos = ImGui::GetWindowPos();

		WindowPos.x -= CursorPos.x;
		WindowPos.y -= CursorPos.y;

		ImVec2 MaxBound = { WindowPos.x + WindowSize.x, WindowPos.y + WindowSize.y };

		m_IsMouseInside = UI::IsMouseHoveringRect(WindowPos, MaxBound);

		ViewportWindowBase::RenderContents();
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
