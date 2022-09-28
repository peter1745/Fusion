#include "GameViewportWindow.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace FusionEditor {

	GameViewportWindow::GameViewportWindow(Fusion::World* InWorld)
		: EditorWindow("GameViewport", 500, 300), m_World(InWorld)
	{
		SetTitle("Game Viewport");
		m_WorldRenderer = Fusion::MakeUnique<Fusion::WorldRenderer>(InWorld, 500, 300);
	}

	void GameViewportWindow::OnRender()
	{
		auto CameraActor = m_World->GetMainCameraActor();

		if (!CameraActor)
			return;

		const Fusion::TransformComponent* CameraTransform = CameraActor->FindComponent<Fusion::TransformComponent>();
		Fusion::CameraComponent* CameraComp = CameraActor->FindComponent<Fusion::CameraComponent>();
		CameraComp->CameraInstance.SetViewportSize(m_ViewportWidth, m_ViewportHeight);

		glm::mat4 ViewMatrix = glm::translate(glm::mat4(1.0f), CameraTransform->Location) * glm::toMat4(CameraTransform->Rotation);
		ViewMatrix = glm::inverse(ViewMatrix);

		m_WorldRenderer->Begin(CameraComp->CameraInstance, ViewMatrix);
		m_WorldRenderer->Render();
		m_WorldRenderer->End();
	}

	void GameViewportWindow::OnUpdate(float InDeltaTime)
	{
		if (m_ViewportWidth != GetWindowWidth() || m_ViewportHeight != GetWindowHeight())
		{
			m_ViewportWidth = GetWindowWidth();
			m_ViewportHeight = GetWindowHeight();

			m_WorldRenderer->SetViewportSize(m_ViewportWidth, m_ViewportHeight);
		}
	}

	void GameViewportWindow::RenderContents()
	{
		auto FinalImage = m_WorldRenderer->GetFinalImage();
		ImTextureID ColorAttachmentImageID = reinterpret_cast<ImTextureID>(FinalImage->GetColorAttachmentID(0));
		ImGui::Image(ColorAttachmentImageID, ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
	}


}
