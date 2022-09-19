#pragma once

#include <string>

#include <imgui.h>

namespace FusionEditor {
	
	class EditorWindow
	{
	public:
		void SetTitle(const std::string& InTitle) { m_Title = InTitle; }
		void Render(bool& InOpen);

	protected:
		EditorWindow(const std::string& InWindowID, uint32_t InInitialWidth = 0, uint32_t InInitialHeight = 0);

	private:
		virtual void RenderContents() = 0;

	private:
		std::string m_WindowID;
		std::string m_Title = "Editor Window";
		uint32_t m_Width;
		uint32_t m_Height;
	};

}
