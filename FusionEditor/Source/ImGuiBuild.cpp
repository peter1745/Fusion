#include "FusionPCH.hpp"

#include <ImGui/backends/imgui_impl_glfw.cpp>

#ifdef FUSION_PLATFORM_WINDOWS
    #include <ImGui/backends/imgui_impl_dx11.cpp>
    #include <ImGui/backends/imgui_impl_dx12.cpp>
#endif
