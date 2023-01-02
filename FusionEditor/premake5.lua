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
        "Fusion",
        "Fission",
        "GLFW",
        "ImGui",
        "yaml-cpp",
        "NFD-Extended",
        "Tracy"
    }

    includedirs
    {
        "%{wks.location}/FusionEditor/Source",
        "%{wks.location}/Fusion/Source",
        "%{wks.location}/Fission/Source",
        "%{IncludeDirs.VulkanMemoryAllocator}",
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

    libdirs {
        "%{LibraryDir.VulkanMemoryAllocator}"
    }

    filter "system:windows"
        systemversion "latest"

        buildoptions { "/EHsc", "/Zc:preprocessor" }

        externalincludedirs {
            (os.getenv("VULKAN_SDK") or "") .. "/Include"
        }

        libdirs {
            "%{wks.location}/ThirdParty/DXC/lib/",
            (os.getenv("VULKAN_SDK") or "") .. "/Lib/"
        }

        links {
            "d3d12.lib",
            "d3d11.lib",
            "dxgi.lib",
            "d3dcompiler.lib",
            "dxcompiler.lib",

            "vulkan-1.lib",

            "VulkanMemoryAllocator.lib",

            "Ws2_32.lib",
            "Dbghelp.lib"
        }

    filter { "system:windows", "configurations:Release" }
        links { "shaderc_combined.lib", "spirv-cross-core.lib", "spirv-cross-cpp.lib", "spirv-cross-glsl.lib" }

    filter { "system:windows", "configurations:Debug" }
        links { "shaderc_combinedd.lib", "spirv-cross-cored.lib", "spirv-cross-cppd.lib", "spirv-cross-glsld.lib" }

    filter "action:vs*"
        postbuildcommands {
            '{COPY} "../ThirdParty/DXC/bin/dxcompiler.dll" "%{cfg.targetdir}"',
            '{COPY} "../ThirdParty/DXC/bin/dxil.dll" "%{cfg.targetdir}"'
        }

    filter "system:linux"
        pic "On"

        debugformat "Dwarf-4"

        removefiles {
            "%{prj.location}/Source/Platform/D3D11/**",
            "%{prj.location}/Source/Platform/D3D12/**"
        }

        libdirs {
            "%{LibraryDir.shaderc}",
            "%{LibraryDir.SPIRV_Cross}"
        }

        links {
            "vulkan",
            "shaderc_combined",
            "spirv-cross",
            "VulkanMemoryAllocator",
        }

        result, err = os.outputof("pkg-config --libs gtk+-3.0")
        linkoptions { result }

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
