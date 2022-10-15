#include "FusionPCH.h"

#ifdef FUSION_PLATFORM_WINDOWS
	#include <ImGui/backends/imgui_impl_win32.cpp>
#else
	#include <ImGui/backends/imgui_impl_glfw.cpp>
#endif

#include <ImGui/backends/imgui_impl_dx11.cpp>
