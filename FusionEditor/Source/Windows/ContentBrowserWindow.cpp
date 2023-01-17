#include "ContentBrowserWindow.hpp"
#include "Fusion/Core/Application.hpp"
#include "AssetSystem/AssetUtils.hpp"

#include <filesystem>

namespace FusionEditor {

	ContentBrowserWindow::ContentBrowserWindow(const std::shared_ptr<Project>& InProject)
		: EditorWindow("Content Browser")
	{
		OnProjectChanged(InProject);
	}

	ContentBrowserWindow::~ContentBrowserWindow()
	{
		m_FileWatcher = nullptr;
		m_CurrentProject = nullptr;
	}

	void ContentBrowserWindow::OnProjectChanged(std::shared_ptr<Project> InProject)
	{
		m_CurrentProject = InProject;

		if (m_CurrentProject == nullptr)
		{
			m_FileWatcher.reset();
			return;
		}

		m_CurrentFolderPath = InProject->BaseDirectory / "Content";
		m_FileWatcher = std::make_unique<filewatch::FileWatch<std::string>>(m_CurrentFolderPath.string(), FUSION_BIND_FUNC(ContentBrowserWindow::OnFileSystemChanged));

		for (auto Entry : std::filesystem::directory_iterator(m_CurrentFolderPath))
			AddFileSystemEntry(Entry.path());
	}

	void ContentBrowserWindow::NavigateToFolder(size_t InFolderIndex)
	{
		m_CurrentFolderPath = m_ContentEntries[InFolderIndex].FilePath;
		m_ContentEntries.clear();

		for (auto Entry : std::filesystem::directory_iterator(m_CurrentFolderPath))
			AddFileSystemEntry(Entry.path());
	}

	void ContentBrowserWindow::RenderContents()
	{
		if (m_CurrentProject == nullptr)
			return;

		const float CellSize = 196.0f;
		const float ScrollBarOffset = 20.0f + ImGui::GetStyle().ScrollbarSize;
		const float PanelWidth = ImGui::GetContentRegionAvail().x - ScrollBarOffset;
		int32_t ColumnCount = glm::max(int32_t(PanelWidth / CellSize), 1);
		ImGui::Columns(ColumnCount, 0, false);

		int32_t ClickedEntryIndex = -1;

		for (size_t Idx = 0; Idx < m_ContentEntries.size(); Idx++)
		{
			const auto& Entry = m_ContentEntries[Idx];

			float ColumnWidth = ImGui::GetColumnWidth();
			std::string Name = Entry.FilePath.filename().string();
			const bool Clicked = ImGui::Button(Name.c_str(), ImVec2(ColumnWidth, ColumnWidth));

			if (Clicked && Entry.IsFolder)
				ClickedEntryIndex = int32_t(Idx);

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				if (!Entry.IsFolder)
				{
					Fusion::AssetHandle Handle = AssetUtils::GetHandleFromFilePath(Entry.FilePath);
					ImGui::SetDragDropPayload("AssetPayload", &Handle, sizeof(Fusion::AssetHandle), ImGuiCond_Once);
				}

				ImGui::Text("%s", Name.c_str());
				ImGui::EndDragDropSource();
			}

			ImGui::NextColumn();
		}

		if (ClickedEntryIndex != -1 && m_ContentEntries[ClickedEntryIndex].IsFolder)
		{
			NavigateToFolder(size_t(ClickedEntryIndex));
		}
	}

	void ContentBrowserWindow::AddFileSystemEntry(const std::filesystem::path& InFilePath)
	{
		if (!std::filesystem::equivalent(m_CurrentFolderPath, InFilePath.parent_path()))
			return;

		if (std::filesystem::is_directory(InFilePath))
		{
			m_ContentEntries.push_back({ InFilePath, true });
		}
		else
		{
			m_ContentEntries.push_back({ InFilePath, false });
		}
	}

	void ContentBrowserWindow::OnFileSystemChanged(const std::string& InFile, const filewatch::Event InEventType)
	{
		Fusion::Application::Get().SubmitToMainThread([this, FilePath = InFile, EventType = InEventType]()
		{
			std::filesystem::path FullPath = m_CurrentProject->BaseDirectory / "Content" / FilePath;

			switch (EventType)
			{
			case filewatch::Event::added:
				AddFileSystemEntry(FullPath);
				break;
			case filewatch::Event::removed:
			{
				break;
			}
			case filewatch::Event::modified:
			{
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
