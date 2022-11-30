project "Fusion"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "Off"

    targetdir (BuildDir .. "/%{prj.name}")
    objdir (IntermediatesDir .. "/%{prj.name}")

    pchheader "FusionPCH.hpp"
    pchsource "Source/FusionPCH.cpp"

    files {
        "Source/**.cpp",
        "Source/**.hpp",
        "Source/**.h",

        "%{wks.location}/ThirdParty/stb_image/include/stb_image/stb_image.cpp",
    }

    includedirs {
        "Source/",
        "%{wks.location}/Fission/Source",
		"%{wks.location}/ThirdParty/glad/include",
        "%{IncludeDirs.GLFW}",
        "%{IncludeDirs.glm}",
        "%{IncludeDirs.spdlog}",
        "%{IncludeDirs.EnTT}",
        "%{IncludeDirs.stb_image}",
        "%{IncludeDirs.TinyGLTF}",
        "%{IncludeDirs.yaml_cpp}"
    }

    defines {
        "GLFW_INCLUDE_NONE",
        "GLM_FORCE_DEPTH_ZERO_TO_ONE",
        "GLM_ENABLE_EXPERIMENTAL",
        "YAML_CPP_STATIC_DEFINE"
    }

    filter "system:windows"
        systemversion "latest"

        includedirs {
            "%{IncludeDirs.DXC}"
        }

    filter "system:linux"
        pic "On"

        removefiles {
            "%{prj.location}/Source/Platform/D3D11/**",
            "%{prj.location}/Source/Platform/D3D12/**",
            "%{prj.location}/Source/Platform/D3DCommon/**"
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
