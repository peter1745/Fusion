#include "FusionEditor.h"
#include <Fusion/EntryPoint.h>
#include <Fusion/Memory/Shared.h>

#include <ImGui/imgui.h>

#include "Windows/GameViewportWindow.h"
#include "Windows/WorldOutlinerWindow.h"
#include "Windows/ActorDetailsWindow.h"
#include "UI/UILibrary.h"

#include <Fusion/Renderer/Mesh.h>
#include <Fusion/Serialization/World/WorldSerializer.h>
#include <Fusion/IO/Keyboard.h>

#include <NFD-Extended/nfd.hpp>

namespace FusionEditor {

	FusionEditorApp::FusionEditorApp(const ApplicationSpecification& specification)
		: Application(specification)
	{
	}

	void FusionEditorApp::OnInit()
	{
		m_ImGuiContext = ImGuiPlatformContext::Create();
		m_ImGuiContext->Init(GetWindow(), m_Renderer->GetContext());
	
		FUSION_CORE_VERIFY(NFD::Init() == NFD_OKAY);

		m_CubeMesh = MeshLoader::LoadMeshFromFile("Resources/Meshes/Cube.gltf");
		m_Texture = TextureLoader::LoadFromFile("Resources/Textures/Test.png");
		
		DummyWorld();
		
		InitWindows();
	}

	void FusionEditorApp::OnUpdate(float DeltaTime)
	{
		m_WindowManager->OnUpdate(DeltaTime);
		m_WindowManager->OnRender();

		DrawUI();
	}

	void FusionEditorApp::OnShutdown()
	{
		NFD::Quit();
		ShutdownImGui();
	}

	void FusionEditorApp::OnEvent(Event& InEvent)
	{
		m_WindowManager->OnEvent(InEvent);
	}

	void FusionEditorApp::InitWindows()
	{
		m_WindowManager = MakeUnique<WindowManager>();
		m_WindowManager->RegisterWindow<WorldOutlinerWindow>(true, m_World);
		m_WindowManager->RegisterWindow<ActorDetailsWindow>(true);
		m_WindowManager->RegisterWindow<EditorViewportWindow>(true, m_World);
		m_WindowManager->RegisterWindow<GameViewportWindow>(true, m_World);
	}

	void FusionEditorApp::DrawUI()
	{
		// Begin ImGui Rendering
		m_ImGuiContext->BeginFrame();

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

		DrawMenuBar();

		ImGui::DockSpace(ImGui::GetID("MainDockspace"));

		m_WindowManager->RenderWindows();

		ImGui::End();

		// End ImGui Rendering
		m_ImGuiContext->EndFrame();
	}

	void FusionEditorApp::DrawMenuBar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Save World..."))
				{
					NFD::UniquePath WorldPath;
					nfdfilteritem_t Filters[] = { { "Fusion World", "fworld" }};
					nfdresult_t Result = NFD::SaveDialog(WorldPath, Filters, 1);

					switch (Result)
					{
					case NFD_OKAY:
					{
						WorldSerializer::SerializeWorld(std::filesystem::path(WorldPath.get()), m_World);
						break;
					}
					case NFD_CANCEL:
					{
						break;
					}
					case NFD_ERROR:
					{
						FUSION_CORE_ERROR("NFD-Extended threw an error: {}", NFD::GetError());
						break;
					}
					}
				}

				if (ImGui::MenuItem("Load World"))
				{
					NFD::UniquePath WorldPath;
					nfdfilteritem_t Filters[] = { { "Fusion World", "fworld" } };
					nfdresult_t Result = NFD::OpenDialog(WorldPath, Filters, 1);

					switch (Result)
					{
					case NFD_OKAY:
					{
						m_World->Clear();
						WorldSerializer::DeserializeWorld(std::filesystem::path(WorldPath.get()), m_World);
						break;
					}
					case NFD_CANCEL:
					{
						break;
					}
					case NFD_ERROR:
					{
						FUSION_CORE_ERROR("NFD-Extended threw an error: {}", NFD::GetError());
						break;
					}
					}
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}

	void FusionEditorApp::ShutdownImGui()
	{
		m_ImGuiContext->Shutdown();
	}

	void FusionEditorApp::DummyWorld()
	{
		m_World = Shared<World>::Create("Dummy World");

		auto Actor01 = m_World->CreateActor("MeshActor");
		auto* SpriteComp = Actor01->AddComponent<Fusion::MeshComponent>();
		SpriteComp->Mesh = m_CubeMesh;
		SpriteComp->Texture = m_Texture;
	}

}

Fusion::Application* Fusion::CreateApplication(int ArgC, char** ArgV)
{
	Fusion::ApplicationSpecification specification = {};
	specification.Title = "Fusion Editor";

	return new FusionEditor::FusionEditorApp(specification);
}
