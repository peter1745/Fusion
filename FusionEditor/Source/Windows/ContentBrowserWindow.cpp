#include "ContentBrowserWindow.h"
#include "Fusion/Core/Application.h"

#include <filesystem>

namespace FusionEditor {

	ContentBrowserWindow::ContentBrowserWindow(const std::shared_ptr<Project>& InProject)
		: EditorWindow("ContentBrowserWindow")
	{
		SetCurrentProject(InProject);
	}

	ContentBrowserWindow::~ContentBrowserWindow()
	{
		m_FileWatcher = nullptr;
		m_CurrentProject = nullptr;
	}

	void ContentBrowserWindow::SetCurrentProject(const std::shared_ptr<Project>& InProject)
	{
		m_CurrentProject = InProject;

		if (m_CurrentProject == nullptr)
		{
			m_FileWatcher.reset();
			return;
		}

		m_FileWatcher = std::make_unique<filewatch::FileWatch<std::string>>(
			(InProject->BaseDirectory / "Content").string(),
			FUSION_BIND_FUNC(ContentBrowserWindow::OnFileSystemChanged));
	}

	void ContentBrowserWindow::ImportAssetFile(const std::filesystem::path& InFilePath)
	{

	}

	void ContentBrowserWindow::RenderContents()
	{
		if (m_CurrentProject == nullptr)
			return;

		for (const auto& Entry : m_ContentEntries)
			ImGui::Text("%s (%s)", Entry.FilePath.string().c_str(), Entry.IsDirectory ? "Directory" : "File");
	}

	void ContentBrowserWindow::OnFileSystemChanged(const std::string& InFile, const filewatch::Event InEventType)
	{
		Fusion::Application::Get().SubmitToMainThread([this, FilePath = InFile, EventType = InEventType]()
		{
			bool IsDirectory = std::filesystem::is_directory(m_CurrentProject->BaseDirectory / "Content" / FilePath);

			switch (EventType)
			{
			case filewatch::Event::added:
				m_ContentEntries.push_back({ FilePath, IsDirectory });
				break;
			case filewatch::Event::removed:
			{
				auto It = std::remove_if(m_ContentEntries.begin(), m_ContentEntries.end(), [&FilePath](const ContentEntry& InEntry)
				{
					return InEntry.FilePath == FilePath;
				});

				if (It != m_ContentEntries.end())
					m_ContentEntries.erase(It);
				break;
			}
			case filewatch::Event::modified:
			{
				FUSION_CORE_INFO("Modified!");
				break;
			}
			case filewatch::Event::renamed_old:
				break;
			case filewatch::Event::renamed_new:
				break;
			}

		});
	}

}
