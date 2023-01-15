project "Fusion"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "Off"

    targetdir (BuildDir .. "/%{prj.name}")
    objdir (IntermediatesDir .. "/%{prj.name}")

    files {
        "Source/**.cpp",
        "Source/**.hpp",
        "Source/**.h",

        "%{wks.location}/FusionTemplateLibrary/Source/**.hpp",

        "%{wks.location}/ThirdParty/stb_image/include/stb_image/stb_image.cpp",
    }

    includedirs {
        "Source/",
        "%{wks.location}/Fission/Source",
        "%{wks.location}/FusionTemplateLibrary/Source",
        "%{IncludeDirs.glad}",
        "%{IncludeDirs.shaderc}",
        "%{IncludeDirs.SPIRV_Cross}",
        "%{IncludeDirs.VulkanMemoryAllocator}",
        "%{IncludeDirs.GLFW}",
        "%{IncludeDirs.glm}",
        "%{IncludeDirs.spdlog}",
        "%{IncludeDirs.EnTT}",
        "%{IncludeDirs.stb_image}",
        "%{IncludeDirs.TinyGLTF}",
        "%{IncludeDirs.yaml_cpp}",
        "%{IncludeDirs.Tracy}"
    }

    defines {
        "GLFW_INCLUDE_NONE",
        "GLM_FORCE_DEPTH_ZERO_TO_ONE",
        "GLM_ENABLE_EXPERIMENTAL",
        "YAML_CPP_STATIC_DEFINE"
    }

    filter "action:vs*"
        pchheader "FusionPCH.hpp"
        pchsource "Source/FusionPCH.cpp"

    filter "action:not vs*"
        pchheader "FusionPCH.hpp"

    filter "system:windows"
        systemversion "latest"

        externalincludedirs {
            (os.getenv("VULKAN_SDK") or "") .. "/Include"
        }

        buildoptions { "/EHsc", "/Zc:preprocessor" }

        includedirs { "%{IncludeDirs.DXC}" }

    filter "system:linux"
        pic "On"
        debugformat "Dwarf-4"

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
