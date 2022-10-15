#include "FusionEditor.h"
#include <Fusion/EntryPoint.h>
#include <Fusion/Memory/Shared.h>

#include <ImGui/imgui.h>
#include <ImGui/backends/imgui_impl_win32.h>
#include <ImGui/backends/imgui_impl_dx11.h>

#include "Windows/GameViewportWindow.h"
#include "Windows/WorldOutlinerWindow.h"
#include "Windows/ActorDetailsWindow.h"
#include <Fusion/Renderer/Mesh.h>


#ifdef FUSION_PLATFORM_WINDOWS
	#include <d3d11.h>
	#include <Platform/D3D11/D3D11Context.h>
	#include <Platform/Windows/WindowsWindow.h>
	extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

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

	static Shared<EditorViewportWindow> s_ViewportWindow;

	FusionEditorApp::FusionEditorApp(const ApplicationSpecification& specification)
		: Application(specification)
	{
#ifdef FUSION_PLATFORM_WINDOWS
		// NOTE(Peter): We have to call the ImGui Win32 Message Proc from Fusions Message Proc, but Fusion has no knowledge of ImGui
		static_cast<WindowsWindow*>(GetWindow().get())->RegisterMessageProc(ImGui_ImplWin32_WndProcHandler);
#endif
	}

	void FusionEditorApp::OnInit()
	{
		InitImGui();
				
		//m_CubeMesh = MeshLoader::LoadMeshFromFile("Resources/Meshes/Cube.gltf");
		
		DummyWorld();
		
		InitWindows();
	}

	void FusionEditorApp::OnUpdate(float DeltaTime)
	{
		//m_WindowManager->OnUpdate(DeltaTime);
		//m_WindowManager->OnRender();

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

		IO.FontDefault = IO.Fonts->AddFontFromFileTTF("Resources/Fonts/Roboto-Regular.ttf", 16.0f);

		InitImGuiStyle();

		auto& Style = ImGui::GetStyle();
		if (IO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			Style.WindowRounding = 0.0f;
			Style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		IO.ConfigWindowsMoveFromTitleBarOnly = true;

		ImGui_ImplWin32_Init(GetWindow()->GetWindowHandle());

		switch (Renderer::CurrentAPI())
		{
		case ERendererAPI::D3D11:
		{
			Shared<D3D11Context> Context = m_Renderer->GetContext().As<D3D11Context>();
			ImGui_ImplDX11_Init(Context->GetDevice(), Context->GetDeviceContext());
			break;
		}
		}
	}

	void FusionEditorApp::InitImGuiStyle()
	{
		auto& UIStyle = ImGui::GetStyle();
		UIStyle.WindowPadding = { 2.0f, 2.0f };
		UIStyle.WindowBorderSize = 0.0f;
		UIStyle.WindowTitleAlign = { 0.5f, 0.5f };
		UIStyle.WindowRounding = 2.0f;
		UIStyle.DisplaySafeAreaPadding = { 0.0f, 6.0f };
		UIStyle.WindowMenuButtonPosition = ImGuiDir_None;

		UIStyle.Colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.0f);
		UIStyle.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.09f, 0.09f, 0.09f, 1.0f);
		UIStyle.Colors[ImGuiCol_Header] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
		UIStyle.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.18f, 0.18f, 0.19f, 1.0f);
		UIStyle.Colors[ImGuiCol_HeaderActive] = ImVec4(0.82f, 0.75f, 0.53f, 1.0f);

		UIStyle.Colors[ImGuiCol_Tab] = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);
		UIStyle.Colors[ImGuiCol_TabActive] = ImVec4(0.2f, 0.2f, 0.19f, 1.0f);
		UIStyle.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.09f, 0.09f, 0.08f, 1.0f);
		UIStyle.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.13f, 1.0f);
		UIStyle.Colors[ImGuiCol_TabHovered] = ImVec4(0.22f, 0.37f, 1.0f, 0.58f);
		UIStyle.TabRounding = 2.0f;

		UIStyle.FramePadding = { 16.0f, 8.0f };
		UIStyle.FrameRounding = 2.0f;
		UIStyle.FrameBorderSize = 0.0f;

		UIStyle.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);

		UIStyle.ItemInnerSpacing = { 6.0f, 4.0f };
		UIStyle.IndentSpacing = 16.0f;
	}

	void FusionEditorApp::InitWindows()
	{
		m_WindowManager = MakeUnique<WindowManager>();
		//m_ViewportWindow = m_WindowManager->RegisterWindow<EditorViewportWindow>(true, m_World.get());
		//m_WindowManager->RegisterWindow<GameViewportWindow>(true, m_World.get());

		m_WindowManager->RegisterWindow<WorldOutlinerWindow>(true, m_World.get());
		m_WindowManager->RegisterWindow<ActorDetailsWindow>(true);
	}

	void FusionEditorApp::DrawUI()
	{
		// Begin ImGui Rendering
		{
			switch (Renderer::CurrentAPI())
			{
			case ERendererAPI::D3D11:
			{
				ImGui_ImplDX11_NewFrame();
				break;
			}
			}

			ImGui_ImplWin32_NewFrame();

			ImGui::NewFrame();
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
			auto& IO = ImGui::GetIO();
			const auto& Window = GetWindow();
			IO.DisplaySize = ImVec2((float)Window->GetWidth(), (float)Window->GetHeight());

			ImGui::Render();

			switch (Renderer::CurrentAPI())
			{
			case ERendererAPI::D3D11:
			{
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
				break;
			}
			}

			if (IO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}

			/*if (IO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				GLFWwindow* ContextBackup = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(ContextBackup);
			}*/
		}
	}

	void FusionEditorApp::ShutdownImGui()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void FusionEditorApp::DummyWorld()
	{
		m_World = MakeUnique<World>();

		auto Actor01 = m_World->CreateActor("MeshActor");
		//auto* SpriteComp = Actor01->AddComponent<Fusion::MeshComponent>();
		//SpriteComp->Mesh = m_CubeMesh;
	}

}

Fusion::Application* Fusion::CreateApplication(int ArgC, char** ArgV)
{
	Fusion::ApplicationSpecification specification = {};

#ifdef _UNICODE
	specification.Title = L"Fusion Editor";
#else
	specification.Title = "Fusion Editor";
#endif

	return new FusionEditor::FusionEditorApp(specification);
}
