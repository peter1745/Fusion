project "glad"
    kind "StaticLib"
    language "C"
    staticruntime "On"

    targetdir (BuildDir .. "/%{prj.name}")
    objdir (IntermediatesDir .. "/%{prj.name}")

    files {
		"src/vulkan.c",
		"include/vulkan/vulkan.h",
		"include/vk_platform.h"
    }

	includedirs {
		"include"
	}

    filter "system:windows"
		systemversion "latest"

    filter "system:linux"
		pic "On"
		systemversion "latest"

    filter "system:macosx"
		pic "On"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"

    filter "configurations:Distribution"
        runtime "Release"
        optimize "Full"
