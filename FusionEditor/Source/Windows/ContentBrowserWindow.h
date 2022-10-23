#pragma once

#include "EditorWindow.h"

#include "Project/Project.h"

#include "IO/FileWatch/FileWatch.hpp"

namespace FusionEditor {

	struct ContentEntry
	{
		std::filesystem::path FilePath;
		bool IsDirectory;
	};

	class ContentBrowserWindow : public EditorWindow
	{
	public:
		ContentBrowserWindow(const std::shared_ptr<Project>& InProject);
		~ContentBrowserWindow();

		void SetCurrentProject(const std::shared_ptr<Project>& InProject);

		void ImportAssetFile(const std::filesystem::path& InFilePath);

	protected:
		virtual void RenderContents() override;

	private:
		void OnFileSystemChanged(const std::string& InFile, const filewatch::Event InEventType);

	private:
		std::shared_ptr<Project> m_CurrentProject = nullptr;
		std::unique_ptr<filewatch::FileWatch<std::string>> m_FileWatcher = nullptr;

		std::vector<ContentEntry> m_ContentEntries;
	};

}
