#include "FusionPCH.h"
#include "Window.h"

#include "Platform/Windows/WindowsWindow.h"

namespace Fusion {

	std::unique_ptr<Window> Window::Create(const WindowProperties& properties)
	{
		return std::make_unique<WindowsWindow>(properties);
	}

}
