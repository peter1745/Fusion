#include "FusionPCH.h"
#include "Window.h"

#include "Platform/Windows/WindowsWindow.h"

namespace Fusion {

	Unique<Window> Window::Create(const WindowSpecification& InSpecification)
	{
#if defined(FUSION_PLATFORM_WINDOWS)
		return MakeUnique<WindowsWindow>(InSpecification);
#else
		FUSION_CORE_VERIFY(false);
#endif
	}

}
