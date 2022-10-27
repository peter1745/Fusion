#include "FusionEditor.hpp"
#include <Fusion/EntryPoint.hpp>
#include <Fusion/Memory/Shared.hpp>

#include <ImGui/imgui.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>

#include "Windows/GameViewportWindow.hpp"
#include "Windows/WorldOutlinerWindow.hpp"
#include "Windows/ActorDetailsWindow.hpp"
#include "Windows/ContentBrowserWindow.hpp"

#include "UI/UILibrary.hpp"

#include "Project/ProjectSerializer.hpp"

#include "AssetSystem/AssetUtils.hpp"

#include <Fusion/Renderer/Mesh.hpp>
#include <Fusion/Serialization/World/WorldSerializer.hpp>
#include <Fusion/IO/Keyboard.hpp>

#include <NFD-Extended/nfd.hpp>

#include <GLFW/glfw3.h>

namespace FusionEditor {

	FusionEditorApp::FusionEditorApp(const ApplicationSpecification& specification)
		: Application(specification, this)
	{
		GLFWwindow* NativeWindow = static_cast<GLFWwindow*>(GetWindow()->GetWindowHandle());
		glfwSetDropCallback(NativeWindow, [](GLFWwindow* InNativeWindow, int32_t InPathCount, const char** InFilePaths)
		{
			Fusion::WindowData* Data = static_cast<Fusion::WindowData*>(glfwGetWindowUserPointer(InNativeWindow));
			FusionEditorApp* App = static_cast<FusionEditorApp*>(Data->Specification.UserData);
			for (int32_t Idx = 0; Idx < InPathCount; Idx++)
				App->ShowImportWindowForFile(InFilePaths[Idx]);
		});
	}

	void FusionEditorApp::OnInit()
	{
		m_ImGuiContext = ImGuiPlatformContext::Create();
		m_ImGuiContext->Init(GetWindow(), m_Renderer->GetContext());
	
		FUSION_CORE_VERIFY(NFD::Init() == NFD_OKAY);
		
		m_World = Shared<World>::Create("Empty World");
		
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

	void FusionEditorApp::ShowImportWindowForFile(const std::filesystem::path& InFilePath)
	{
		if (!m_CurrentProject)
			return;

		Shared<ContentBrowserWindow> ContentBrowser = m_WindowManager->GetWindowOfType<ContentBrowserWindow>();

		if (!ContentBrowser->IsMouseInside())
		{
			FUSION_CLIENT_WARN("Tried dropping asset outside of the content browser window!");
			return;
		}

		EAssetType Type = AssetUtils::GetTypeFromExtension(InFilePath);
		const auto& OutputPath = m_WindowManager->GetWindowOfType<ContentBrowserWindow>()->GetCurrentFolderPath();
		m_WindowManager->OpenWindowByID(m_AssetImporterWindows[Type]->GetID());
		m_AssetImporterWindows[Type]->SetImportPaths(InFilePath, OutputPath);
	}

	void FusionEditorApp::InitWindows()
	{
		m_WindowManager = MakeUnique<WindowManager>();
		m_WindowManager->RegisterWindow<WorldOutlinerWindow>(true, m_World);
		m_WindowManager->RegisterWindow<ActorDetailsWindow>(true);
		m_WindowManager->RegisterWindow<EditorViewportWindow>(true, m_World);
		m_WindowManager->RegisterWindow<GameViewportWindow>(true, m_World);
		m_WindowManager->RegisterWindow<ContentBrowserWindow>(true, nullptr);

		m_AssetImporterWindows[EAssetType::Mesh] = m_WindowManager->RegisterWindow<MeshImporterWindow>(false);
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

		DrawNewProjectPopup();

		ImGui::DockSpace(ImGui::GetID("MainDockspace"));

		m_WindowManager->RenderWindows();

		if (!m_PopupToOpen.empty())
		{
			ImGui::OpenPopup(m_PopupToOpen.c_str());
			m_PopupToOpen = "";
		}

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
				if (ImGui::MenuItem("New Project..."))
				{
					m_PopupToOpen = "New Project";
				}

				if (ImGui::MenuItem("Open Project..."))
				{
					NFD::UniquePath ProjectPath;
					nfdfilteritem_t Filters[] = { { "Fusion Project", "fproj" } };
					nfdresult_t Result = NFD::OpenDialog(ProjectPath, Filters, 1);

					switch (Result)
					{
					case NFD_OKAY:
					{
						LoadProject(ProjectPath.get());
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

	void FusionEditorApp::CreateProject(const std::string& InName, const std::filesystem::path& InLocation)
	{
		// TODO: Unload current project
		std::filesystem::path ProjectBaseDirectory = InLocation / InName;

		std::filesystem::create_directory(ProjectBaseDirectory);
		std::filesystem::create_directory(ProjectBaseDirectory / "Content");
		
		std::shared_ptr<Project> NewProject = std::make_shared<Project>();
		NewProject->Name = InName;
		NewProject->BaseDirectory = ProjectBaseDirectory;

		ProjectSerializer::Serialize(NewProject);

		SetTitle(std::format("Fusion Editor - {}", InName));

		m_CurrentProject = NewProject;

		m_WindowManager->GetWindowOfType<ContentBrowserWindow>()->SetCurrentProject(m_CurrentProject);
	}

	void FusionEditorApp::LoadProject(const std::filesystem::path& InProjectPath)
	{
		// TODO: Unload current project
		m_CurrentProject = ProjectSerializer::Deserialize(InProjectPath);

		SetTitle(std::format("Fusion Editor - {}", m_CurrentProject->Name));

		m_WindowManager->GetWindowOfType<ContentBrowserWindow>()->SetCurrentProject(m_CurrentProject);

		for (auto It : std::filesystem::recursive_directory_iterator(m_CurrentProject->BaseDirectory / "Content"))
		{
			
		}
	}

	void FusionEditorApp::DrawNewProjectPopup()
	{
		static std::string s_ProjectName = "";
		static std::string s_ProjectLocation = "";

		if (ImGui::BeginPopupModal("New Project", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
		{
			bool IsValidLocation = !s_ProjectLocation.empty() && !std::filesystem::exists(std::filesystem::path(s_ProjectLocation) / s_ProjectName);

			ImGui::SetNextItemWidth(-1);
			ImGui::InputTextWithHint("##NewProjectName", "Project Name", &s_ProjectName);

			if (!IsValidLocation)
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));

			ImGui::InputTextWithHint("##NewProjectLocation", "Project Location", &s_ProjectLocation, ImGuiInputTextFlags_ReadOnly);
			bool OpenFolderDialogue = ImGui::IsItemClicked(ImGuiMouseButton_Left);

			if (!IsValidLocation)
				ImGui::PopStyleColor();

			ImGui::SameLine();
			OpenFolderDialogue |= ImGui::Button("Open...");

			if (OpenFolderDialogue)
			{
				NFD::UniquePath ProjectLocation;
				nfdresult_t Result = NFD::PickFolder(ProjectLocation);

				switch (Result)
				{
				case NFD_OKAY:
				{
					s_ProjectLocation = ProjectLocation.get();
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

			UI::BeginDisabled(!IsValidLocation);
			if (ImGui::Button("Create"))
			{
				CreateProject(s_ProjectName, s_ProjectLocation);
				ImGui::CloseCurrentPopup();
			}
			UI::EndDisabled();

			ImGui::SameLine();

			if (ImGui::Button("Close"))
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}
	}

}

Fusion::Application* Fusion::CreateApplication(int ArgC, char** ArgV)
{
	Fusion::ApplicationSpecification specification = {};
	specification.Title = "Fusion Editor";
	
	return new FusionEditor::FusionEditorApp(specification);
}
