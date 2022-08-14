#include "FusionPCH.h"
#include "Window.h"

#include "Platform/Windows/WindowsWindow.h"

namespace Fusion {

	Unique<Window> Window::Create(const WindowSpecification& specification)
	{
#if defined(FUSION_PLATFORM_WINDOWS)
		return MakeUnique<WindowsWindow>(specification);
#else
		static_assert(false);
#endif
	}

}
