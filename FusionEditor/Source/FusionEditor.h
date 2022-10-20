#pragma once

#include <Fusion.h>
#include <Fusion/Renderer/Texture.h>
#include <Fusion/World/World.h>
#include <Fusion/Renderer/WorldRenderer.h>
#include <Fusion/Renderer/Mesh.h>

#include "Windows/WindowManager.h"
#include "Windows/EditorViewportWindow.h"

#include "Renderer/ViewportCamera.h"

#include "Platform/D3D11/ImGuiPlatformContextD3D11.h"

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
		void InitWindows();
		void DrawUI();
		void DrawMenuBar();
		void ShutdownImGui();

		void DummyWorld();

	private:
		Shared<Mesh> m_CubeMesh = nullptr;

		Shared<World> m_World = nullptr;
		Unique<WindowManager> m_WindowManager = nullptr;
		Shared<EditorViewportWindow> m_ViewportWindow = nullptr;

		std::unique_ptr<ImGuiPlatformContext> m_ImGuiContext = nullptr;
	};

}
