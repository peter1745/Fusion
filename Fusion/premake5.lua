project "Fusion"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "Off"

    targetdir (BuildDir .. "/%{prj.name}")
    objdir (IntermediatesDir .. "/%{prj.name}")

    pchheader "FusionPCH.h"
    pchsource "Source/FusionPCH.cpp"

    files
    {
        "Source/**.cpp",
        "Source/**.h",

        "%{wks.location}/ThirdParty/stb_image/include/stb_image/stb_image.cpp"
    }

    includedirs
    {
        "Source/",
        "%{IncludeDirs.GLFW}",
        "%{IncludeDirs.glm}",
        "%{IncludeDirs.spdlog}",
        "%{IncludeDirs.glad}",
        "%{IncludeDirs.EnTT}",
        "%{IncludeDirs.stb_image}"
    }

    defines { "GLFW_INCLUDE_NONE" }

    filter "system:windows"
        systemversion "latest"

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
