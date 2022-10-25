#pragma once

#include "ViewportWindowBase.hpp"
#include "Renderer/ViewportCamera.hpp"

#include <Fusion/Events/KeyboardEvents.hpp>

namespace FusionEditor {

	enum class EGizmoType { None = -1, Translate, Rotate, Scale };
	enum class EGizmoSpace { Local, World };

	class EditorViewportWindow : public ViewportWindowBase
	{
	public:
		EditorViewportWindow(const Fusion::Shared<Fusion::World>& InWorld);

		virtual void OnUpdate(float InDeltaTime) override;
		virtual void OnEvent(Fusion::Event& InEvent) override;

	protected:
		virtual void RenderContents() override;

	private:
		virtual void RenderWorld() override;
		virtual void OnResize(uint32_t InWidth, uint32_t InHeight) override;

		void MainpulateGizmos();

		bool OnKeyPressed(Fusion::KeyPressedEvent& InEvent);

		void OnSelectionChanged(Fusion::Shared<Fusion::Actor> InActor);

	private:
		ViewportCamera m_Camera;
		EGizmoType m_ActiveGizmoType = EGizmoType::None;
		EGizmoSpace m_GizmoSpace = EGizmoSpace::World;

		Fusion::Shared<Fusion::Actor> m_SelectedActor = nullptr;
	};

}
