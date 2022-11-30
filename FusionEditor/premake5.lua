project "FusionEditor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "Off"

    targetdir (BuildDir .. "/%{prj.name}")
    objdir (IntermediatesDir .. "/%{prj.name}")

    files
    {
        "Source/**.cpp",
        "Source/**.hpp",
        "Source/**.h"
    }

    links
    {
        "ImGui",
        "Fusion",
        "Fission",
        "GLFW",
        "yaml-cpp",
        "NFD-Extended",
        "Tracy"
    }

    includedirs
    {
        "%{wks.location}/FusionEditor/Source",
        "%{wks.location}/Fusion/Source",
        "%{wks.location}/Fission/Source",
        "%{IncludeDirs.GLFW}",
        "%{IncludeDirs.glm}",
        "%{IncludeDirs.spdlog}",
        "%{IncludeDirs.ImGui}",
        "%{IncludeDirs.ImGui}/ImGui", -- Fix for ImGui includes
        "%{IncludeDirs.EnTT}",
        "%{IncludeDirs.stb_image}",
        "%{IncludeDirs.yaml_cpp}",
        "%{IncludeDirs.NFD_Extended}",
        "%{IncludeDirs.Tracy}"
    }

    defines {
        "GLM_FORCE_DEPTH_ZERO_TO_ONE",
        "GLM_ENABLE_EXPERIMENTAL",
        "YAML_CPP_STATIC_DEFINE"
    }

    filter "system:windows"
        systemversion "latest"

        links
        {
            "d3d12.lib",
            "d3d11.lib",
            "dxgi.lib",
            "d3dcompiler.lib",

            "%{wks.location}/ThirdParty/DXC/lib/dxcompiler.lib"
        }

        postbuildcommands 
	    {
	    	'{COPY} "../ThirdParty/DXC/bin/dxcompiler.dll" "%{cfg.targetdir}"',
	    	'{COPY} "../ThirdParty/DXC/bin/dxil.dll" "%{cfg.targetdir}"'
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

        defines {
            "TRACY_ENABLE",
            "TRACY_ON_DEMAND"
        }

    filter "configurations:Distribution"
        defines "FUSION_DIST"
        runtime "Release"
        optimize "Full"
        conformancemode "On"

        defines {
            "TRACY_ENABLE",
            "TRACY_ON_DEMAND"
        }
