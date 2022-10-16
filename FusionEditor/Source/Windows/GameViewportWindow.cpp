#include "GameViewportWindow.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace FusionEditor {

	GameViewportWindow::GameViewportWindow(Fusion::World* InWorld)
		: ViewportWindowBase("GameViewport", InWorld)
	{
		SetTitle("Game Viewport");
	}

	void GameViewportWindow::RenderWorld()
	{
		auto CameraActor = m_World->GetMainCameraActor();

		if (!CameraActor)
			return;

		const Fusion::TransformComponent* CameraTransform = CameraActor->FindComponent<Fusion::TransformComponent>();
		Fusion::CameraComponent* CameraComp = CameraActor->FindComponent<Fusion::CameraComponent>();

		glm::mat4 ViewMatrix = glm::translate(glm::mat4(1.0f), CameraTransform->Location) * glm::toMat4(CameraTransform->Rotation);
		ViewMatrix = glm::inverse(ViewMatrix);

		m_WorldRenderer->Begin(CameraComp->CameraInstance, ViewMatrix);
		m_WorldRenderer->Render();
		m_WorldRenderer->End();
	}

	void GameViewportWindow::OnResize(uint32_t InWidth, uint32_t InHeight)
	{
		auto CameraActor = m_World->GetMainCameraActor();
		if (CameraActor)
			CameraActor->FindComponent<Fusion::CameraComponent>()->CameraInstance.SetViewportSize(InWidth, InHeight);
	}

}
