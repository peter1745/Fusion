#include "EditorWindow.h"
#include "UI/UILibrary.h"

namespace FusionEditor {

	EditorWindow::EditorWindow(const std::string& InWindowID, uint32_t InInitialWidth /*= 0*/, uint32_t InInitialHeight /*= 0*/)
		: m_WindowID(InWindowID), m_Width(InInitialWidth), m_Height(InInitialHeight)
	{
	}

	void EditorWindow::CheckMouseIsInside()
	{
		if (GImGui->CurrentWindow == nullptr)
			return;

		ImVec2 CursorPos = ImGui::GetCursorPos();
		ImVec2 WindowSize = ImGui::GetWindowSize();
		ImVec2 WindowPos = ImGui::GetWindowPos();

		WindowPos.x -= CursorPos.x;
		WindowPos.y -= CursorPos.y;

		ImVec2 MaxBound = { WindowPos.x + WindowSize.x, WindowPos.y + WindowSize.y };

		m_IsMouseInside = UI::IsMouseHoveringRect(WindowPos, MaxBound);
	}

	void EditorWindow::RenderUI(bool& InOpen)
	{
		std::string WindowTitle = std::format("{0}##{1}", m_Title, m_WindowID);
		const bool IsActive = ImGui::Begin(WindowTitle.c_str(), &InOpen);

		m_IsTabActive = IsActive && InOpen;

		if (IsActive)
		{
			CheckMouseIsInside();

			ImVec2 ContentRegionSize = ImGui::GetContentRegionAvail();

			RenderContents();

			m_Width = uint32_t(ContentRegionSize.x);
			m_Height = uint32_t(ContentRegionSize.y);
		}

		if (!IsActive || !InOpen)
		{
			m_IsMouseInside = false;
		}

		ImGui::End();
	}

}
