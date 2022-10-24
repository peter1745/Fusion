#pragma once

#include "EditorWindow.h"

#include "Project/Project.h"

#include "IO/FileWatch/FileWatch.hpp"

namespace FusionEditor {

	struct ContentBrowserEntry
	{
		std::filesystem::path FilePath;
		bool IsFolder;
	};

	class ContentBrowserWindow : public EditorWindow
	{
	public:
		ContentBrowserWindow(const std::shared_ptr<Project>& InProject);
		~ContentBrowserWindow();

		void SetCurrentProject(const std::shared_ptr<Project>& InProject);

		void NavigateToFolder(size_t InFolderIndex);

		const std::filesystem::path& GetCurrentFolderPath() const { return m_CurrentFolderPath; }

	protected:
		virtual void RenderContents() override;

	private:
		void AddFileSystemEntry(const std::filesystem::path& InFilePath);
		void OnFileSystemChanged(const std::string& InFile, const filewatch::Event InEventType);

	private:
		std::shared_ptr<Project> m_CurrentProject = nullptr;
		std::unique_ptr<filewatch::FileWatch<std::string>> m_FileWatcher = nullptr;

		std::filesystem::path m_CurrentFolderPath = "";
		std::vector<ContentBrowserEntry> m_ContentEntries;
	};

}
