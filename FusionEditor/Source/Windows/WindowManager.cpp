#include "WindowManager.h"

namespace FusionEditor {

	void WindowManager::RenderWindows()
	{
		for (auto& [WindowID, Data] : m_Windows)
		{
			if (!Data.IsOpen)
				continue;

			Data.Window->Render(Data.IsOpen);
		}
	}

}
