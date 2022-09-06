#include "FusionEditor.h"
#include <Fusion/EntryPoint.h>
#include <Fusion/Memory/Shared.h>
#include <Fusion/Renderer/RenderData.h>

#include <ImGui/imgui.h>
#include <ImGui/backends/imgui_impl_glfw.h>
#include <ImGui/backends/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

namespace FusionEditor {

	static Fusion::Vertex s_TriangleVertices[] = {
		{ {  0.5f,  0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }, // Top-Right
		{ {  0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, // Bottom-Right
		{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }, // Bottom-Left
		{ { -0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f, 1.0f } }  // Top-Left
	};

	static uint32_t s_TriangleIndices[] = { 0, 1, 2, 2, 3, 0 };

	FusionEditorApp::FusionEditorApp(const Fusion::ApplicationSpecification& specification)
		: Application(specification)
	{
	}

	void FusionEditorApp::OnInit()
	{
		InitImGui();

		Fusion::ShaderSpecification ShaderSpec;
		ShaderSpec.VertexFilePath = "Resources/Shaders/VertexShader.vert";
		ShaderSpec.FragmentFilePath = "Resources/Shaders/FragmentShader.frag";
		m_Shader = Fusion::Shader::Create(ShaderSpec);

		Fusion::VertexBufferLayout VertexLayout({
			{ 0, Fusion::ShaderDataType::Float3, offsetof(Fusion::Vertex, Position) },
			{ 1, Fusion::ShaderDataType::Float4, offsetof(Fusion::Vertex, Color) }
		});
		m_VertexBuffer = Fusion::VertexBuffer::Create(4 * sizeof(Fusion::Vertex), s_TriangleVertices, VertexLayout);

		m_IndexBuffer = Fusion::IndexBuffer::Create(6, s_TriangleIndices);

		Fusion::FramebufferSpecification FramebufferSpec;
		FramebufferSpec.Width = GetWindow()->GetWidth();
		FramebufferSpec.Height = GetWindow()->GetHeight();
		FramebufferSpec.ColorAttachments.push_back({ Fusion::EFramebufferAttachmentFormat::RGBA8 });
		FramebufferSpec.DepthAttachment.Format = Fusion::EFramebufferAttachmentFormat::Depth24Stencil8;
		m_ViewportFramebuffer = Fusion::Framebuffer::Create(FramebufferSpec);
	}

	void FusionEditorApp::OnUpdate(float DeltaTime)
	{
		// Scene Rendering
		m_ViewportFramebuffer->Bind();
		Fusion::Renderer::Begin();
		Fusion::Renderer::DrawIndexed(m_VertexBuffer, m_IndexBuffer, m_Shader);
		Fusion::Renderer::End();
		m_ViewportFramebuffer->Unbind();

		DrawUI();
	}

	void FusionEditorApp::OnShutdown()
	{
		ShutdownImGui();
	}

	void FusionEditorApp::InitImGui()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		auto& IO = ImGui::GetIO();
		IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		IO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark();

		auto& Style = ImGui::GetStyle();
		if (IO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			Style.WindowRounding = 0.0f;
			Style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		GLFWwindow* NativeWindow = static_cast<GLFWwindow*>(GetWindow()->GetNativeWindow());
		ImGui_ImplGlfw_InitForOpenGL(NativeWindow, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void FusionEditorApp::DrawUI()
	{
		// Begin ImGui Rendering
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			// NOTE(Peter): This is mainly to clear the framebuffer
			Fusion::Renderer::Begin();
		}

		ImGui::Begin("Viewport");
		ImTextureID ColorAttachmentImageID = reinterpret_cast<ImTextureID>(m_ViewportFramebuffer->GetColorAttachmentID(0));
		ImGui::Image(ColorAttachmentImageID, ImVec2(500, 500), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::End();

		// End ImGui Rendering
		{
			Fusion::Renderer::End();

			auto& IO = ImGui::GetIO();
			const auto& Window = GetWindow();
			IO.DisplaySize = ImVec2((float)Window->GetWidth(), (float)Window->GetHeight());

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			if (IO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				GLFWwindow* ContextBackup = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(ContextBackup);
			}
		}
	}

	void FusionEditorApp::ShutdownImGui()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

}

Fusion::Application* Fusion::CreateApplication(int ArgC, char** ArgV)
{
	Fusion::ApplicationSpecification specification = {};
	specification.Title = "Fusion Editor";

	return new FusionEditor::FusionEditorApp(specification);
}
