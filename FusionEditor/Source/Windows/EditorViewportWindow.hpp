#pragma once

#include "ViewportWindowBase.hpp"
#include "Renderer/ViewportCamera.hpp"

#include <Fusion/Events/KeyboardEvents.hpp>

#include <Fusion/Renderer/StagingBuffer.hpp>

namespace FusionEditor {

	enum class EGizmoType { None = -1, Translate, Rotate, Scale };
	enum class EGizmoSpace { Local, World };

	class EditorViewportWindow : public ViewportWindowBase
	{
	public:
		EditorViewportWindow(const Fusion::Shared<Fusion::World>& InWorld, Fusion::DescriptorHeap* InDescriptorHeap);

		virtual void OnUpdate(float InDeltaTime) override;
		virtual void OnEvent(Fusion::Event& InEvent) override;

	protected:
		virtual void RenderContents() override;

	private:
		virtual void RenderWorld() override;
		virtual void OnResize(uint32_t InWidth, uint32_t InHeight) override;

		bool OnKeyPressed(Fusion::KeyPressedEvent& InEvent);

		void OnSelectionChanged(Fusion::Shared<Fusion::Actor> InActor);

	private:
		ViewportCamera m_Camera;
		EGizmoType m_ActiveGizmoType = EGizmoType::None;
		EGizmoSpace m_GizmoSpace = EGizmoSpace::World;

		Fusion::Shared<Fusion::Actor> m_SelectedActor = nullptr;

		Fusion::Unique<Fusion::StagingBuffer> m_StagingBuffer;
		bool m_ShouldCopyFromBuffer = false;
	};

}
