#include "FusionEditor.h"
#include <Fusion/EntryPoint.h>
#include <Fusion/Memory/Shared.h>

#include <ImGui/imgui.h>
#include <ImGui/backends/imgui_impl_glfw.h>
#include <ImGui/backends/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#include "Windows/WorldOutlinerWindow.h"
#include "Windows/ActorDetailsWindow.h"

namespace FusionEditor {

	struct VertexData
	{
		glm::vec3 Position;
		glm::vec2 TextureCoordinate;
	};

	static VertexData s_TriangleVertices[] = {
		{ {  0.5f,  0.5f, 0.0f }, { 1.0f, 1.0f } }, // Top-Right
		{ {  0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f } }, // Bottom-Right
		{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f } }, // Bottom-Left
		{ { -0.5f,  0.5f, 0.0f }, { 0.0f, 1.0f } }  // Top-Left
	};

	static uint32_t s_TriangleIndices[] = { 0, 1, 2, 2, 3, 0 };

	static Shared<ViewportWindow> s_ViewportWindow;

	FusionEditorApp::FusionEditorApp(const ApplicationSpecification& specification)
		: Application(specification)
	{
	}

	void FusionEditorApp::OnInit()
	{
		InitImGui();

		ShaderSpecification ShaderSpec;
		ShaderSpec.VertexFilePath = "Resources/Shaders/TextureVertexShader.glsl";
		ShaderSpec.FragmentFilePath = "Resources/Shaders/TextureFragmentShader.glsl";
		m_Shader = Shader::Create(ShaderSpec);

		VertexBufferLayout VertexLayout({
			{ 0, ShaderDataType::Float3, offsetof(VertexData, Position) },
			{ 1, ShaderDataType::Float2, offsetof(VertexData, TextureCoordinate) }
		});
		m_VertexBuffer = VertexBuffer::Create(4 * sizeof(VertexData), s_TriangleVertices, VertexLayout);

		m_IndexBuffer = IndexBuffer::Create(6, s_TriangleIndices);

		FramebufferSpecification FramebufferSpec;
		FramebufferSpec.Width = GetWindow()->GetWidth();
		FramebufferSpec.Height = GetWindow()->GetHeight();
		FramebufferSpec.ColorAttachments.push_back({ EFramebufferAttachmentFormat::RGBA8 });
		FramebufferSpec.DepthAttachment.Format = EFramebufferAttachmentFormat::Depth24Stencil8;
		m_ViewportFramebuffer = Framebuffer::Create(FramebufferSpec);

		m_Texture = Texture2D::LoadFromFile("Resources/Textures/Test.png");
		m_Texture->Bind(0);

		m_World = MakeUnique<World>();

		InitWindows();
	}

	void FusionEditorApp::OnUpdate(float DeltaTime)
	{
		// Scene Rendering
		m_ViewportFramebuffer->Bind();
		Renderer::Begin();
		Renderer::GetActiveCommandBuffer()->CmdBindShader(m_Shader);
		m_Shader->Set("MainTexture", m_Texture);
		Renderer::DrawIndexed(m_VertexBuffer, m_IndexBuffer, m_Shader);
		Renderer::End();
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

	void FusionEditorApp::InitWindows()
	{
		m_WindowManager = MakeUnique<WindowManager>();
		m_ViewportWindow = m_WindowManager->RegisterWindow<ViewportWindow>(true, m_ViewportFramebuffer);

		m_WindowManager->RegisterWindow<WorldOutlinerWindow>(true, m_World.get());
		m_WindowManager->RegisterWindow<ActorDetailsWindow>(true);
	}

	void FusionEditorApp::DrawUI()
	{
		// Begin ImGui Rendering
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			// NOTE(Peter): This is mainly to clear the framebuffer
			Renderer::Begin();
		}

		ImGuiViewport* MainViewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(MainViewport->Pos);
		ImGui::SetNextWindowSize(MainViewport->Size);
		ImGui::SetNextWindowViewport(MainViewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGuiWindowFlags DockspaceWindowFlags =
			ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::Begin("MainDockspaceWindow", nullptr, DockspaceWindowFlags);
		ImGui::PopStyleVar(3);

		ImGui::DockSpace(ImGui::GetID("MainDockspace"));

		m_WindowManager->RenderWindows();

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
