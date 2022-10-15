#include "FusionPCH.h"
#include "Window.h"
#include "Fusion/IO/Keyboard.h"
#include "Fusion/IO/GLFWKeyMappings.h"

#include "Platform/Windows/WindowsWindow.h"

#include <GLFW/glfw3.h>

namespace Fusion {

	Unique<Window> Window::Create(const WindowSpecification& InSpecification)
	{
#ifdef FUSION_PLATFORM_WINDOWS
		return MakeUnique<WindowsWindow>(InSpecification);
#else
		return nullptr;
#endif
	}

}
