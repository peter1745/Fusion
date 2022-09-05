project "glad"
    kind "StaticLib"
    language "C"
    staticruntime "Off"

    targetdir (BuildDir .. "/%{prj.name}")
    objdir (IntermediatesDir .. "/%{prj.name}")

    files
    {
        "src/gl.c",
        "include/glad/gl.h",
        "include/KHR/khrplatform.h"
    }

    includedirs
    {
        "include"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        conformancemode "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"
        conformancemode "On"

    filter "configurations:Distribution"
        runtime "Release"
        optimize "Full"
        conformancemode "On"
