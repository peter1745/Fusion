#include "EditorViewportWindow.hpp"

#include "UI/UILibrary.hpp"
#include "UI/ImGuizmo.h"
#include "WindowManager.hpp"
#include "WorldOutlinerWindow.hpp"

#include <Fusion/Renderer/GraphicsContext.hpp>
#include <Fusion/Renderer/Renderer.hpp>
#include <Fusion/IO/Mouse.hpp>

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <spdlog/fmt/fmt.h>

namespace FusionEditor {

	EditorViewportWindow::EditorViewportWindow(const Fusion::Shared<Fusion::World>& InWorld, const ActorSelectionManager& InSelectionCtx)
	    : ViewportWindowBase("MainViewport", InWorld), m_Camera(1280, 720), m_SelectionManager(InSelectionCtx)
	{
		SetTitle("Viewport");

		m_SelectionManager->AddSelectionCallback(FUSION_BIND_FUNC(EditorViewportWindow::OnActorSelected));
		m_SelectionManager->AddDeselectionCallback(FUSION_BIND_FUNC(EditorViewportWindow::OnActorDeselected));

		//Fusion::StagingBufferInfo StagingBufferCreateInfo = {};
		//StagingBufferCreateInfo.Format = Fusion::EFormat::RG32UInt;
		//StagingBufferCreateInfo.Size = sizeof(Fusion::ActorID);
		//m_StagingBuffer = Fusion::StagingBuffer::Create(StagingBufferCreateInfo);
	}

	void EditorViewportWindow::OnUpdate(float InDeltaTime)
	{
		ViewportWindowBase::OnUpdate(InDeltaTime);

		if (m_ShouldCopyFromBuffer)
		{
			Fusion::ActorID SelectedActorID = 0;
			Fusion::Byte* BufferStart = m_StagingBuffer->Map();
			memcpy(&SelectedActorID, BufferStart, sizeof(Fusion::ActorID));
			m_StagingBuffer->Unmap(BufferStart);
			m_ShouldCopyFromBuffer = false;

			auto Actor = m_World->FindActorWithID(SelectedActorID);

			m_SelectionManager->DeselectAll();

			if (Actor)
				m_SelectionManager->Select(SelectedActorID, Actor);
		}

		m_Camera.SetActive(IsMouseInside() && IsTabActive());
		m_Camera.OnUpdate(InDeltaTime);
	}

	void EditorViewportWindow::OnEvent(Fusion::Event& InEvent)
	{
		Fusion::EventDispatcher Dispatcher(InEvent);
		Dispatcher.Dispatch<Fusion::KeyPressedEvent>([this](auto& InKeyEvent) {
			return OnKeyPressed(InKeyEvent);
		});
	}

	void EditorViewportWindow::RenderContents()
	{
		if (ImGui::Button("Create Physics Actors"))
		{
			for (int32_t X = -25; X < 25; X++)
			{
				for (int32_t Z = -25; Z < 25; Z++)
				{
					auto NewActor = m_World->CreateActor(fmt::format("Actor-{}-{}", X * 2, Z * 2));
					auto* ActorTransform = NewActor->FindComponent<Fusion::TransformComponent>();
					ActorTransform->Location = { X * 3, 20, Z * 3 };
					NewActor->AddComponent<Fusion::PhysicsBodyComponent>();
					NewActor->AddComponent<Fusion::SphereShapeComponent>();
					NewActor->AddComponent<Fusion::MeshComponent>()->MeshHandle = Fusion::AssetHandle(68537410238160412);
				}
			}

			auto GroundActor = m_World->CreateActor("Ground");
			auto* ActorTransform = GroundActor->FindComponent<Fusion::TransformComponent>();
			ActorTransform->Location = { 0, -150, 0 };
			ActorTransform->Scale = { 100, 10, 100 };
			GroundActor->AddComponent<Fusion::PhysicsBodyComponent>()->Mass = 0.0f;
			GroundActor->AddComponent<Fusion::BoxShapeComponent>()->HalfSize = { 50.0f, 5.0f, 50.0f };
			GroundActor->AddComponent<Fusion::MeshComponent>()->MeshHandle = Fusion::AssetHandle(23188905328565712);
		}

		if (m_World->GetState() & Fusion::WorldStates::None)
		{
			if (ImGui::Button("Play"))
			{
				m_BackupWorld = Fusion::World::Copy(m_World);
				m_World->StartSimulation();
			}
		}
		else
		{
			if (ImGui::Button("Stop"))
			{
				m_World->StopSimulation();
				m_World->Restore(m_BackupWorld);
				m_BackupWorld = nullptr;
			}
		}

		ViewportWindowBase::RenderContents();

		const auto& SelectedActors = m_SelectionManager->All();

		if (m_ActiveGizmoType == EGizmoType::None || SelectedActors.empty())
			return;

		ImGuizmo::OPERATION CurrentOperation = (ImGuizmo::OPERATION) -1;
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

		const glm::mat4& ViewMatrix = m_Camera.GetViewMatrix();
		const glm::mat4& ProjectionMatrix = m_Camera.GetProjectionMatrix();

		glm::mat4 GizmoTransform = CalculateGizmoTransform();
		glm::mat4 DeltaTransform = glm::mat4(1.0f);

		bool TransformChanged = ImGuizmo::Manipulate(
		    glm::value_ptr(ViewMatrix),
		    glm::value_ptr(ProjectionMatrix),
		    CurrentOperation,
		    CurrentMode,
		    glm::value_ptr(GizmoTransform),
		    glm::value_ptr(DeltaTransform));

		if (TransformChanged)
		{
			glm::vec3 DeltaTranslation, DeltaScale;
			glm::quat DeltaRotation;
			glm::vec3 DeltaSkew;
			glm::vec4 DeltaPerspective;
			glm::decompose(DeltaTransform, DeltaScale, DeltaRotation, DeltaTranslation, DeltaSkew, DeltaPerspective);

			for (auto [ActorID, Actor] : SelectedActors)
			{
				Fusion::TransformComponent* ActorTransformComp = Actor->FindComponent<Fusion::TransformComponent>();
				ActorTransformComp->Location += DeltaTranslation;
				ActorTransformComp->Scale *= DeltaScale;
				ActorTransformComp->SetRotation(DeltaRotation * ActorTransformComp->GetRotation());
			}
		}
	}

	void EditorViewportWindow::RenderWorld()
	{
		auto* CmdList = Fusion::Renderer::GetCurrent().GetCurrentCommandList();
		
		uint32_t FrameIndex = Fusion::Renderer::GetCurrent().GetCurrentFrame();
		
		m_WorldRenderer->Begin(m_Camera, m_Camera.GetViewMatrix());
		m_WorldRenderer->Render();
		m_WorldRenderer->End();
		
		auto MousePos = Fusion::Mouse::Get().GetPosition();
		MousePos.x -= m_MinRenderBoundX;
		MousePos.y -= m_MinRenderBoundY;
		
		const bool MouseXInside = MousePos.x > m_MinRenderBoundX && MousePos.x < m_RenderWidth;
		const bool MouseYInside = MousePos.y > m_MinRenderBoundY && MousePos.y < m_RenderHeight;
		
		if ((MouseXInside && MouseYInside) && IsTabActive() && Fusion::Mouse::Get().IsButtonPressed(Fusion::EMouseButton::Left))
		{
			/*Fusion::Shared<Fusion::Image2D> ColorPickingImage = m_RenderTexture->GetImage(1, FrameIndex);
			ColorPickingImage->Transition(CmdList, Fusion::ImageStates::CopySrc);
		
			Fusion::CopyRegionInfo CopyRegion = {};
			CopyRegion.Left = MousePos.x;
			CopyRegion.Top = MousePos.y;
			CopyRegion.Right = MousePos.x + 1;
			CopyRegion.Bottom = MousePos.y + 1;
			m_StagingBuffer->CopyFrom(CmdList, ColorPickingImage, CopyRegion);
			m_ShouldCopyFromBuffer = true;
		
			ColorPickingImage->Transition(CmdList, Fusion::ImageStates::RenderTarget);*/
		}
	}

	void EditorViewportWindow::OnResize(uint32_t InWidth, uint32_t InHeight)
	{
		if (InWidth == 0 || InHeight == 0)
			return;

		m_Camera.SetViewportSize(InWidth, InHeight);
	}

	glm::mat4 EditorViewportWindow::CalculateGizmoTransform() const
	{
		const auto& SelectedActors = m_SelectionManager->All();

		glm::vec3 MedianLocation(0.0f), MedianScale(0.0f);

		for (const auto& [ActorID, Actor] : SelectedActors)
		{
			const Fusion::TransformComponent* ActorTransform = Actor->FindComponent<Fusion::TransformComponent>();
			MedianLocation += ActorTransform->Location;
			MedianScale += ActorTransform->Scale;
		}

		MedianLocation /= SelectedActors.size();
		MedianScale /= SelectedActors.size();

		return glm::translate(glm::mat4(1.0f), MedianLocation) * glm::scale(glm::mat4(1.0f), MedianScale);
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

	void EditorViewportWindow::OnActorSelected(Fusion::Shared<Fusion::Actor> InActor)
	{
	}

	void EditorViewportWindow::OnActorDeselected(Fusion::Shared<Fusion::Actor> InActor)
	{
	}

}
