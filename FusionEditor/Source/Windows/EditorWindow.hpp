#pragma once

#include "Fusion/Memory/Shared.hpp"
#include "Fusion/Events/Event.hpp"

#include <string>

#include <imgui.h>

namespace FusionEditor {

	enum class EMenuBarLocation { None, View };
	
	class EditorWindow : public Fusion::SharedObject
	{
	public:
		virtual ~EditorWindow() = default;

		void SetTitle(const std::string& InTitle) { m_Title = InTitle; }
		void RenderUI(bool& InOpen);

		virtual void OnRender() {}
		virtual void OnUpdate([[maybe_unused]] float InDeltaTime) {}
		virtual void OnEvent([[maybe_unused]] Fusion::Event& InEvent) {}

		uint32_t GetWindowWidth() const { return m_Width; }
		uint32_t GetWindowHeight() const { return m_Height; }

		bool IsTabActive() const { return m_IsTabActive; }
		bool IsMouseInside() const { return m_IsMouseInside; }

		uint32_t GetID() const { return m_WindowID; }

		virtual constexpr EMenuBarLocation GetMenuBarLocation() const = 0;

		std::string_view GetTitle() const { return m_Title; }

		const ImVec2& GetMinBound() const { return m_MinBound; }
		const ImVec2& GetMaxBound() const { return m_MaxBound; }

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

		ImVec2 m_MinBound, m_MaxBound;

		bool m_IsTabActive = false;
		bool m_IsMouseInside = false;

		friend class WindowManager;
	};

}
