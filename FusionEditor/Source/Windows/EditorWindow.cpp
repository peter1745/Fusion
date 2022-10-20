#include "EditorWindow.h"

#include <format>

namespace FusionEditor {

	EditorWindow::EditorWindow(const std::string& InWindowID, uint32_t InInitialWidth /*= 0*/, uint32_t InInitialHeight /*= 0*/)
		: m_WindowID(InWindowID), m_Width(InInitialWidth), m_Height(InInitialHeight)
	{
	}

	void EditorWindow::RenderUI(bool& InOpen)
	{
		std::string WindowTitle = std::format("{0}##{1}", m_Title, m_WindowID);
		if (ImGui::Begin(WindowTitle.c_str(), &InOpen))
		{
			m_IsFocused = ImGui::IsWindowFocused();

			ImVec2 ContentRegionSize = ImGui::GetContentRegionAvail();

			RenderContents();

			m_Width = uint32_t(ContentRegionSize.x);
			m_Height = uint32_t(ContentRegionSize.y);
		}

		ImGui::End();
	}

}
