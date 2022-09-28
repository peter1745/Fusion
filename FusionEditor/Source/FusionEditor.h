#pragma once

#include <Fusion.h>
#include <Fusion/Renderer/Texture.h>
#include <Fusion/World/World.h>
#include <Fusion/Renderer/WorldRenderer.h>
#include <Fusion/Renderer/Mesh.h>

#include "Windows/WindowManager.h"
#include "Windows/EditorViewportWindow.h"

#include "Renderer/ViewportCamera.h"

namespace FusionEditor {

	using namespace Fusion;

	class FusionEditorApp : public Application
	{
	public:
		FusionEditorApp(const ApplicationSpecification& specification);

		virtual void OnInit() override;
		virtual void OnUpdate(float DeltaTime) override;
		virtual void OnShutdown() override;

	private:
		void InitImGui();
		void InitImGuiStyle();
		void InitWindows();
		void DrawUI();
		void ShutdownImGui();

		void DummyWorld();

	private:
		Shared<Mesh> m_CubeMesh = nullptr;

		Unique<World> m_World = nullptr;
		Unique<WindowManager> m_WindowManager = nullptr;
		Shared<EditorViewportWindow> m_ViewportWindow = nullptr;
	};

}
