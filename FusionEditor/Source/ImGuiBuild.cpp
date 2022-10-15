#include "FusionPCH.h"

#define IMGUI_IMPL_OPENGL_LOADER_GLAD

#ifdef FUSION_PLATFORM_WINDOWS
	#include <ImGui/backends/imgui_impl_win32.cpp>
#else
	#include <ImGui/backends/imgui_impl_glfw.cpp>
#endif

#include <ImGui/backends/imgui_impl_opengl3.cpp>
#include <ImGui/backends/imgui_impl_dx11.cpp>
