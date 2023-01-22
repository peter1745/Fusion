#pragma once

#include <Fusion.hpp>
#include <Fusion/Renderer/Renderer.hpp>
//#include <Fusion/Renderer/Texture.hpp>
#include <Fusion/World/World.hpp>
#include <Fusion/Renderer/Mesh.hpp>
#include <Fusion/IO/TextureLoader.hpp>
#include <Fusion/AssetSystem/Asset.hpp>

#include "Windows/WindowManager.hpp"
#include "Windows/EditorViewportWindow.hpp"
#include "Windows/AssetSystem/MeshImporterWindow.hpp"

#include "Renderer/ViewportCamera.hpp"
#include "Renderer/WorldRenderer.hpp"

#include "UI/ImGuiPlatformContext.hpp"
#include "Project/Project.hpp"

namespace FusionEditor {

	using namespace Fusion;

	class FusionEditorApp : public Application
	{
	public:
		FusionEditorApp(const ApplicationSpecification& specification);

		virtual void OnInit() override;
		virtual void OnUpdate(float DeltaTime) override;
		virtual void OnShutdown() override;
		virtual void OnEvent(Event& InEvent) override;

		void ShowImportWindowForFile(const std::filesystem::path& InFilePath);

		void ScheduleProjectForLoad(const std::filesystem::path& InProjectPath)
		{
			m_ScheduledProject = InProjectPath;
		}

	private:
		void InitWindows();
		void DrawUI();
		void DrawMenuBar();
		void ShutdownImGui();

		void CreateProject(const std::string& InName, const std::filesystem::path& InDirectory);
		void LoadProject(const std::filesystem::path& InProjectPath);

	private:
		void DrawNewProjectPopup();

	private:
		ActorSelectionManager m_ActorSelectionManager = nullptr;

		Unique<Renderer> m_Renderer = nullptr;

		Shared<Mesh> m_CubeMesh = nullptr;

		Shared<World> m_World = nullptr;
		Shared<World> m_BackupWorld = nullptr;
		Unique<WindowManager> m_WindowManager = nullptr;

		std::unordered_map<EAssetType, Shared<AssetImporterWindow>> m_AssetImporterWindows;

		std::unique_ptr<ImGuiPlatformContext> m_ImGuiContext = nullptr;

		std::shared_ptr<Project> m_CurrentProject;

		std::string m_PopupToOpen = "";

		std::filesystem::path m_ScheduledProject = "";
	};

}
