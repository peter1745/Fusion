#pragma once

#include "ViewportWindowBase.hpp"
#include "IO/Selection/SelectionManager.hpp"
#include "Renderer/ViewportCamera.hpp"

#include <Fusion/Events/KeyboardEvents.hpp>

namespace FusionEditor {

	enum class EGizmoType { None = -1, Translate, Rotate, Scale };
	enum class EGizmoSpace { Local, World };

	class EditorViewportWindow : public ViewportWindowBase
	{
	public:
		EditorViewportWindow(const Fusion::Shared<Fusion::World>& InWorld, const ActorSelectionManager& InSelectionCtx);

		virtual void OnUpdate(float InDeltaTime) override;
		virtual void OnEvent(Fusion::Event& InEvent) override;

	protected:
		virtual void RenderContents() override;

	private:
		virtual void RenderWorld() override;
		virtual void OnResize(uint32_t InWidth, uint32_t InHeight) override;

		glm::mat4 CalculateGizmoTransform() const;

		bool OnKeyPressed(Fusion::KeyPressedEvent& InEvent);

		void OnActorSelected(Fusion::Shared<Fusion::Actor> InActor);
		void OnActorDeselected(Fusion::Shared<Fusion::Actor> InActor);

	private:
		ViewportCamera m_Camera;

		ActorSelectionManager m_SelectionManager;

		EGizmoType m_ActiveGizmoType = EGizmoType::None;
		EGizmoSpace m_GizmoSpace = EGizmoSpace::World;

		//Fusion::Unique<Fusion::StagingBuffer> m_StagingBuffer;
		bool m_ShouldCopyFromBuffer = false;

		Fusion::Shared<Fusion::World> m_BackupWorld = nullptr;
	};

}
