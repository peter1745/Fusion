#pragma once

#include "EditorWindow.h"

#include <Fusion/Renderer/WorldRenderer.h>

namespace FusionEditor {

	class GameViewportWindow : public EditorWindow
	{
	public:
		GameViewportWindow(Fusion::World* InWorld);

		virtual void OnRender() override;
		virtual void OnUpdate(float InDeltaTime) override;

	private:
		virtual void RenderContents() override;

	private:
		Fusion::World* m_World = nullptr;
		Fusion::Unique<Fusion::WorldRenderer> m_WorldRenderer = nullptr;

		float m_ViewportWidth = 0.0f, m_ViewportHeight = 0.0f;
	};

}
