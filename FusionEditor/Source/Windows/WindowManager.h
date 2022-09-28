#pragma once

#include "EditorWindow.h"

#include "Fusion/Core/TypeInfo.h"
#include "Fusion/Memory/Shared.h"

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

		template<typename TWindow, typename... TWindowParams>
		requires std::derived_from<TWindow, EditorWindow>&& std::constructible_from<TWindow, TWindowParams...>
		Fusion::Shared<TWindow> RegisterWindow(bool InOpenByDefault, TWindowParams&&... InParams)
		{
			uint32_t WindowID = Fusion::TypeInfo<TWindow>().HashCode();

			if (m_Windows.find(WindowID) != m_Windows.end())
			{
				FUSION_CLIENT_ERROR("Tried to register a window of type '{}' which is already registered!", Fusion::TypeInfo<TWindow>().Name());
				return nullptr;
			}

			WindowData& Data = m_Windows[WindowID];
			Data.Window = Fusion::Shared<TWindow>::Create(std::forward<TWindowParams>(InParams)...);
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
			uint32_t WindowID = Fusion::TypeInfo<TWindow>().HashCode();

			if (m_Windows.find(WindowID) == m_Windows.end())
			{
				FUSION_CLIENT_ERROR("Failed to find window of type '{}'!", Fusion::TypeInfo<TWindow>().Name());
				return nullptr;
			}

			return m_Windows.at(WindowID).Window;
		}

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
