#include "EditorViewportWindow.hpp"

#include "UI/UILibrary.hpp"
#include "UI/ImGuizmo.h"
#include "WindowManager.hpp"
#include "WorldOutlinerWindow.hpp"

#include <Fusion/IO/Mouse.hpp>

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace FusionEditor {

	EditorViewportWindow::EditorViewportWindow(const Fusion::Shared<Fusion::World>& InWorld)
		: ViewportWindowBase("MainViewport", InWorld), m_Camera(1280, 720)
	{
		SetTitle("Viewport");

		auto WorldOutliner = WindowManager::Get()->GetWindowOfType<WorldOutlinerWindow>();
		WorldOutliner->GetSelectionCallbackList().AddFunction(FUSION_BIND_FUNC(EditorViewportWindow::OnSelectionChanged));

		m_ActorIDTexture = Texture2D::Create();
	}

	void EditorViewportWindow::OnUpdate(float InDeltaTime)
	{
		ViewportWindowBase::OnUpdate(InDeltaTime);

		FUSION_CORE_INFO("Mouse Inside: {}", IsMouseInside());

		if (IsMouseInside() && IsTabActive() && Fusion::Mouse::Get().IsButtonPressed(Fusion::EMouseButton::Left))
		{
			auto MousePos = Fusion::Mouse::Get().GetPosition();
			MousePos.x -= GetMinBound().x;
			MousePos.y -= GetMinBound().y;

			Fusion::ActorID ID = m_RenderTexture->ReadPixel(1, uint32_t(MousePos.x), uint32_t(MousePos.y));
			OnSelectionChanged(m_World->FindActorWithID(ID));
		}

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

	void EditorViewportWindow::RenderContents()
	{
		ViewportWindowBase::RenderContents();

		if (m_ActiveGizmoType == EGizmoType::None || m_SelectedActor == nullptr)
			return;

		ImGuizmo::OPERATION CurrentOperation = (ImGuizmo::OPERATION)-1;
		switch (m_ActiveGizmoType)
		{
		case EGizmoType::None:
			return;
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

		Fusion::TransformComponent* ActorTransformComp = m_SelectedActor->FindComponent<Fusion::TransformComponent>();

		const glm::mat4& ViewMatrix = m_Camera.GetViewMatrix();
		const glm::mat4& ProjectionMatrix = m_Camera.GetProjectionMatrix();
		glm::mat4 ActorTransformMatrix =
			  glm::translate(glm::mat4(1.0f), ActorTransformComp->Location)
			* glm::toMat4(ActorTransformComp->GetRotation())
			* glm::scale(glm::mat4(1.0f), ActorTransformComp->Scale);

		bool TransformChanged = ImGuizmo::Manipulate(
			glm::value_ptr(ViewMatrix),
			glm::value_ptr(ProjectionMatrix),
			CurrentOperation,
			CurrentMode,
			glm::value_ptr(ActorTransformMatrix));

		if (TransformChanged)
		{
			glm::quat Rotation;
			glm::vec3 Skew;
			glm::vec4 Perspective;
			glm::decompose(ActorTransformMatrix, ActorTransformComp->Scale, Rotation, ActorTransformComp->Location, Skew, Perspective);
			ActorTransformComp->SetRotation(Rotation);
		}
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
			case Fusion::EKeyCode::L:
			{
				m_GizmoSpace = (m_GizmoSpace == EGizmoSpace::Local) ? EGizmoSpace::World : EGizmoSpace::Local;
				ConsumeKeyPress = true;
				break;
			}
			default:
			{
				ConsumeKeyPress = false;
				break;
			}
			}
		}

		return ConsumeKeyPress;
	}

	void EditorViewportWindow::OnSelectionChanged(Fusion::Shared<Fusion::Actor> InActor)
	{
		m_SelectedActor = InActor;
	}

}
