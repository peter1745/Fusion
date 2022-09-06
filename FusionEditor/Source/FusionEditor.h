#pragma once

#include <Fusion.h>

namespace FusionEditor {

	class FusionEditorApp : public Fusion::Application
	{
	public:
		FusionEditorApp(const Fusion::ApplicationSpecification& specification);

		virtual void OnInit() override;
		virtual void OnUpdate(float DeltaTime) override;
		virtual void OnShutdown() override;

	private:
		void InitImGui();
		void DrawUI();
		void ShutdownImGui();

	private:
		Fusion::Shared<Fusion::Framebuffer> m_ViewportFramebuffer;
		Fusion::Shared<Fusion::VertexBuffer> m_VertexBuffer;
		Fusion::Shared<Fusion::IndexBuffer> m_IndexBuffer;
		Fusion::Shared<Fusion::Shader> m_Shader;
	};

}
