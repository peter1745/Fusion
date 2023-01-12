#include "EditorWindow.hpp"
#include "UI/UILibrary.hpp"

#include <Fusion/IO/Mouse.hpp>

namespace FusionEditor {

	EditorWindow::EditorWindow(const std::string& InTitle, uint32_t InInitialWidth /*= 0*/, uint32_t InInitialHeight /*= 0*/)
		: m_Title(InTitle), m_Width(InInitialWidth), m_Height(InInitialHeight)
	{
	}

	void EditorWindow::CheckMouseIsInside()
	{
		if (GImGui->CurrentWindow == nullptr)
			return;

		auto ViewportOffset = ImGui::GetCursorPos();
		auto WindowSize = ImGui::GetWindowSize();

		m_MinBound = ImGui::GetWindowPos();
		m_MinBound.x -= ViewportOffset.x;
		m_MinBound.y -= ViewportOffset.y;

		m_MaxBound = { m_MinBound.x + WindowSize.x, m_MinBound.y + WindowSize.y };

		m_IsMouseInside = UI::IsMouseHoveringRect(m_MinBound, m_MaxBound);
	}

	void EditorWindow::RenderUI(bool& InOpen)
	{
		std::string WindowTitle = fmt::format("{0}##{1}", m_Title, m_WindowID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
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
		ImGui::PopStyleVar();
	}
}
