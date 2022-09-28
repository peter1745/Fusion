#pragma once

#include "EditorWindow.h"
#include "Renderer/ViewportCamera.h"

#include <Fusion/Renderer/WorldRenderer.h>

namespace FusionEditor {

	class EditorViewportWindow : public EditorWindow
	{
	public:
		EditorViewportWindow(Fusion::World* InWorld);

		virtual void OnRender() override;
		virtual void OnUpdate(float InDeltaTime) override;

	private:
		virtual void RenderContents() override;
		
	private:
		Fusion::Unique<Fusion::WorldRenderer> m_WorldRenderer = nullptr;
		ViewportCamera m_Camera;

		float m_ViewportWidth = 0.0f, m_ViewportHeight = 0.0f;
	};

}
