project "NFD-Extended"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "Off"

    targetdir (BuildDir .. "/%{prj.name}")
    objdir (IntermediatesDir .. "/%{prj.name}")

    files { "include/NFD-Extended/include/nfd.h", "include/NFD-Extended/include/nfd.hpp" }
    includedirs { "include/" }

    filter "system:windows"
		systemversion "latest"

        files { "src/nfd_win.cpp" }

    filter "system:linux"
		pic "On"
		systemversion "latest"

        files { "src/nfd_gtk.cpp" }

    filter "system:macosx"
		pic "On"

        files { "src/nfd_cocoa.m" }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"

    filter "configurations:Distribution"
        runtime "Release"
        optimize "Full"
