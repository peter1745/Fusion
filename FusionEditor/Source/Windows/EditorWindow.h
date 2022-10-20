#pragma once

#include "Fusion/Memory/Shared.h"

#include <string>

#include <imgui.h>

namespace FusionEditor {
	
	class EditorWindow : public Fusion::SharedObject
	{
	public:
		void SetTitle(const std::string& InTitle) { m_Title = InTitle; }
		void RenderUI(bool& InOpen);

		virtual void OnRender() {}
		virtual void OnUpdate(float InDeltaTime) {}

		uint32_t GetWindowWidth() const { return m_Width; }
		uint32_t GetWindowHeight() const { return m_Height; }

		bool IsTabFocused() const { return m_IsTabFocused; }

	protected:
		EditorWindow(const std::string& InWindowID, uint32_t InInitialWidth = 0, uint32_t InInitialHeight = 0);

		virtual void RenderContents() = 0;

	private:
		std::string m_WindowID;
		std::string m_Title = "Editor Window";
		uint32_t m_Width;
		uint32_t m_Height;

		bool m_IsTabFocused = false;
	};

}
