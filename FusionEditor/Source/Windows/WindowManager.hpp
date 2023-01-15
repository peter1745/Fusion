#pragma once

#include "EditorWindow.hpp"

#include "Fusion/IO/Logging.hpp"
#include "Fusion/Memory/Shared.hpp"

#include <FTL/DemangledTypeName.hpp>

#include <unordered_map>
#include <concepts>

namespace FusionEditor {

	struct WindowData
	{
		Fusion::Shared<EditorWindow> Window;
		bool IsOpen = false;
	};

	class WindowManager
	{
	public:
		WindowManager()
		{
			s_Instance = this;
		}

		void OpenWindowByID(uint32_t InWindowID)
		{
			if (m_Windows.find(InWindowID) == m_Windows.end())
			{
				Fusion::LogWarn("Fusion Editor", "Tried to open a window using an invalid ID!");
				return;
			}

			m_Windows.at(InWindowID).IsOpen = true;
		}

		void CloseWindowByID(uint32_t InWindowID)
		{
			if (m_Windows.find(InWindowID) == m_Windows.end())
			{
				Fusion::LogWarn("Fusion Editor", "Tried to open a window using an invalid ID!");
				return;
			}

			m_Windows.at(InWindowID).IsOpen = false;
		}

		template<typename TWindow, typename... TWindowParams>
		requires std::derived_from<TWindow, EditorWindow>&& std::constructible_from<TWindow, TWindowParams...>
		Fusion::Shared<TWindow> RegisterWindow(bool InOpenByDefault, TWindowParams&&... InParams)
		{
			uint32_t WindowID = FTL::DemangledTypeName<TWindow>().HashCode();

			if (m_Windows.find(WindowID) != m_Windows.end())
			{
				Fusion::LogError("Fusion Editor", "Tried to register a window of type '{}' which is already registered!", FTL::DemangledTypeName<TWindow>().Name());
				return nullptr;
			}

			WindowData& Data = m_Windows[WindowID];
			Data.Window = Fusion::Shared<TWindow>::Create(std::forward<TWindowParams>(InParams)...);
			Data.Window->m_WindowID = WindowID;
			Data.IsOpen = InOpenByDefault;
			return Data.Window;
		}

		template<typename TWindow, typename... TWindowParams>
		requires std::derived_from<TWindow, EditorWindow> && std::constructible_from<TWindow, TWindowParams...>
		Fusion::Shared<TWindow> RegisterWindow(TWindowParams&&... InParams)
		{
			return RegisterWindow<TWindow>(false, std::forward<TWindowParams>(InParams)...);
		}

		template<typename TWindow>
		requires std::derived_from<TWindow, EditorWindow>
		Fusion::Shared<TWindow> GetWindowOfType() const
		{
			uint32_t WindowID = FTL::DemangledTypeName<TWindow>().HashCode();

			if (m_Windows.find(WindowID) == m_Windows.end())
			{
				Fusion::LogError("Fusion Editor", "Failed to find window of type '{}'!", FTL::DemangledTypeName<TWindow>().Name());
				return nullptr;
			}

			return m_Windows.at(WindowID).Window;
		}

		void OnEvent(Fusion::Event& InEvent);

		const std::unordered_map<uint32_t, WindowData>& GetAllWindows() const { return m_Windows; }

	public:
		inline static WindowManager* Get() { return s_Instance; }

	private:
		void RenderWindows();
		void OnRender();
		void OnUpdate(float InDeltaTime);

	private:
		std::unordered_map<uint32_t, WindowData> m_Windows;

		inline static WindowManager* s_Instance = nullptr;

		friend class FusionEditorApp;
	};

}
