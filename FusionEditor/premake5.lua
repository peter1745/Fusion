project "FusionEditor"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "Off"

    targetdir (BuildDir .. "/%{prj.name}")
    objdir (IntermediatesDir .. "/%{prj.name}")

    files
    {
        "Source/**.cpp",
        "Source/**.h"
    }

    links
    {
        "ImGui",
        "Fusion",
        "GLFW"
    }

    includedirs
    {
        "%{wks.location}/FusionEditor/Source",
        "%{wks.location}/Fusion/Source",
        "%{IncludeDirs.GLFW}",
        "%{IncludeDirs.glm}",
        "%{IncludeDirs.spdlog}",
        "%{IncludeDirs.ImGui}",
        "%{IncludeDirs.ImGui}/ImGui", -- Fix for ImGui includes
        "%{IncludeDirs.EnTT}"
    }

    filter "system:windows"
        systemversion "latest"

        links
        {
            "d3d11.lib",
            "dxgi.lib",
            "d3dcompiler.lib"
        }

    filter "configurations:Debug"
        defines "FUSION_DEBUG"
        runtime "Debug"
        symbols "On"
        conformancemode "On"

    filter "configurations:Release"
        defines "FUSION_RELEASE"
        runtime "Release"
        optimize "On"
        conformancemode "On"

    filter "configurations:Distribution"
        defines "FUSION_DIST"
        runtime "Release"
        optimize "Full"
        conformancemode "On"
