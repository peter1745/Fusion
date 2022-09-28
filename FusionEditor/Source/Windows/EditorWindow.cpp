#include "EditorWindow.h"

#include <format>

namespace FusionEditor {

	EditorWindow::EditorWindow(const std::string& InWindowID, uint32_t InInitialWidth /*= 0*/, uint32_t InInitialHeight /*= 0*/)
		: m_WindowID(InWindowID), m_Width(InInitialWidth), m_Height(InInitialHeight)
	{
	}

	void EditorWindow::Render(bool& InOpen)
	{
		std::string WindowTitle = std::format("{0}##{1}", m_Title, m_WindowID);
		//ImGui::SetNextWindowSize(ImVec2(float(m_Width), float(m_Height)), ImGuiCond_Once);
		if (ImGui::Begin(WindowTitle.c_str(), &InOpen))
			RenderContents();
		ImGui::End();
	}

}
