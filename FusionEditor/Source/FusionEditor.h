#pragma once

#include <Fusion.h>
#include <Fusion/Renderer/Texture.h>
#include <Fusion/World/World.h>

#include "Windows/WindowManager.h"
#include "Windows/ViewportWindow.h"

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
		void InitWindows();
		void DrawUI();
		void ShutdownImGui();

	private:
		Shared<Framebuffer> m_ViewportFramebuffer;
		Shared<VertexBuffer> m_VertexBuffer;
		Shared<IndexBuffer> m_IndexBuffer;
		Shared<Shader> m_Shader;
		Shared<Texture2D> m_Texture;

		Unique<World> m_World = nullptr;
		Unique<WindowManager> m_WindowManager = nullptr;
		Shared<ViewportWindow> m_ViewportWindow = nullptr;
	};

}
