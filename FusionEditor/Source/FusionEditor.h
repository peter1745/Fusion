#pragma once

#include <Fusion.h>
#include <Fusion/Renderer/Texture.h>
#include <Fusion/World/World.h>
#include <Fusion/Renderer/WorldRenderer.h>
#include <Fusion/Renderer/Mesh.h>
#include <Fusion/IO/TextureLoader.h>
#include <Fusion/AssetSystem/Asset.h>

#include "Windows/WindowManager.h"
#include "Windows/EditorViewportWindow.h"
#include "Windows/AssetSystem/MeshImporterWindow.h"

#include "Renderer/ViewportCamera.h"

#include "UI/ImGuiPlatformContext.h"
#include "Project/Project.h"

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
		Shared<Mesh> m_CubeMesh = nullptr;
		Shared<Texture2D> m_Texture = nullptr;

		Shared<World> m_World = nullptr;
		Unique<WindowManager> m_WindowManager = nullptr;

		std::unordered_map<EAssetType, Shared<AssetImporterWindow>> m_AssetImporterWindows;

		std::unique_ptr<ImGuiPlatformContext> m_ImGuiContext = nullptr;

		std::shared_ptr<Project> m_CurrentProject;

		std::string m_PopupToOpen = "";
	};

}
