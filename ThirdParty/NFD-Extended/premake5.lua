project "NFD-Extended"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "Off"

    targetdir (BuildDir .. "/%{prj.name}")
    objdir (IntermediatesDir .. "/%{prj.name}")

    files { "include/NFD-Extended/nfd.h", "include/NFD-Extended/nfd.hpp" }
    includedirs { "include/" }

    filter "system:windows"
    	systemversion "latest"
        files { "src/nfd_win.cpp" }

    filter "system:linux"
        pic "On"
        systemversion "latest"
        debugformat "Dwarf-4"

        files { "src/nfd_gtk.cpp" }

        result, err = os.outputof("pkg-config --cflags gtk+-3.0")
        buildoptions { result }

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
