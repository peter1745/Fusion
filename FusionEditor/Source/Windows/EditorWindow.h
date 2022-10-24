#pragma once

#include "Fusion/Memory/Shared.h"
#include "Fusion/Events/Event.h"

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
		virtual void OnEvent(Fusion::Event& InEvent) {}

		uint32_t GetWindowWidth() const { return m_Width; }
		uint32_t GetWindowHeight() const { return m_Height; }

		bool IsTabActive() const { return m_IsTabActive; }
		bool IsMouseInside() const { return m_IsMouseInside; }

		uint32_t GetID() const { return m_WindowID; }

	protected:
		EditorWindow(const std::string& InTitle, uint32_t InInitialWidth = 0, uint32_t InInitialHeight = 0);

		virtual void RenderContents() = 0;

	private:
		void CheckMouseIsInside();

	private:
		uint32_t m_WindowID;
		std::string m_Title = "Editor Window";
		uint32_t m_Width;
		uint32_t m_Height;

		bool m_IsTabActive = false;
		bool m_IsMouseInside = false;

		friend class WindowManager;
	};

}
