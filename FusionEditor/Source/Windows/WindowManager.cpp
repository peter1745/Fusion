#include "WindowManager.h"

namespace FusionEditor {

	void WindowManager::RenderWindows()
	{
		for (auto& [WindowID, Data] : m_Windows)
		{
			if (!Data.IsOpen)
				continue;

			Data.Window->RenderUI(Data.IsOpen);
		}
	}

	void WindowManager::OnRender()
	{
		for (auto& [WindowID, Data] : m_Windows)
		{
			if (!Data.IsOpen)
				continue;

			Data.Window->OnRender();
		}
	}

	void WindowManager::OnUpdate(float InDeltaTime)
	{
		for (auto& [WindowID, Data] : m_Windows)
		{
			if (!Data.IsOpen)
				continue;

			Data.Window->OnUpdate(InDeltaTime);
		}
	}

}
