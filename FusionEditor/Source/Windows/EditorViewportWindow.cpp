#include "EditorViewportWindow.h"

#include "UI/UILibrary.h"
#include "UI/ImGuizmo.h"

#include <Fusion/IO/Mouse.h>

#include <glm/gtc/type_ptr.hpp>

namespace FusionEditor {

	EditorViewportWindow::EditorViewportWindow(const Fusion::Shared<Fusion::World>& InWorld)
		: ViewportWindowBase("MainViewport", InWorld), m_Camera(1280, 720)
	{
		SetTitle("Viewport");
	}

	void EditorViewportWindow::OnUpdate(float InDeltaTime)
	{
		ViewportWindowBase::OnUpdate(InDeltaTime);

		m_Camera.SetActive(IsMouseInside() && IsTabActive());
		m_Camera.OnUpdate(InDeltaTime);
	}

	void EditorViewportWindow::OnEvent(Fusion::Event& InEvent)
	{
		Fusion::EventDispatcher Dispatcher(InEvent);
		Dispatcher.Dispatch<Fusion::KeyPressedEvent>([this](auto& InKeyEvent)
		{
			return OnKeyPressed(InKeyEvent);
		});
	}

	static glm::mat4 s_Transform = glm::mat4(1.0f);

	void EditorViewportWindow::RenderContents()
	{
		ViewportWindowBase::RenderContents();

		if (m_ActiveGizmoType == EGizmoType::None)
			return;

		ImGuizmo::OPERATION CurrentOperation = (ImGuizmo::OPERATION)-1;
		switch (m_ActiveGizmoType)
		{
		case EGizmoType::Translate:
			CurrentOperation = ImGuizmo::TRANSLATE;
			break;
		case EGizmoType::Rotate:
			CurrentOperation = ImGuizmo::ROTATE;
			break;
		case EGizmoType::Scale:
			CurrentOperation = ImGuizmo::SCALE;
			break;
		}

		ImGuizmo::MODE CurrentMode = m_GizmoSpace == EGizmoSpace::Local ? ImGuizmo::LOCAL : ImGuizmo::WORLD;

		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

		const glm::mat4& ViewMatrix = m_Camera.GetViewMatrix();
		const glm::mat4& ProjectionMatrix = m_Camera.GetProjectionMatrix();

		ImGuizmo::Manipulate(
			glm::value_ptr(ViewMatrix),
			glm::value_ptr(ProjectionMatrix),
			CurrentOperation,
			CurrentMode,
			glm::value_ptr(s_Transform));
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

	bool EditorViewportWindow::OnKeyPressed(Fusion::KeyPressedEvent& InEvent)
	{
		bool ConsumeKeyPress = false;

		if (Fusion::Mouse::Get().GetVisibility() == Fusion::EMouseVisibility::Visible)
		{
			switch (InEvent.GetKeyCode())
			{
			case Fusion::EKeyCode::Q:
			{
				m_ActiveGizmoType = EGizmoType::None;
				ConsumeKeyPress = true;
				break;
			}
			case Fusion::EKeyCode::W:
			{
				m_ActiveGizmoType = EGizmoType::Translate;
				ConsumeKeyPress = true;
				break;
			}
			case Fusion::EKeyCode::E:
			{
				m_ActiveGizmoType = EGizmoType::Rotate;
				ConsumeKeyPress = true;
				break;
			}
			case Fusion::EKeyCode::R:
			{
				m_ActiveGizmoType = EGizmoType::Scale;
				ConsumeKeyPress = true;
				break;
			}
			}
		}

		return ConsumeKeyPress;
	}

}
