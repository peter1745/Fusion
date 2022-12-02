project "Tracy"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
    staticruntime "Off"

    toolset "clang"

	targetdir (BuildDir .. "/%{prj.name}")
	objdir (IntermediatesDir .. "/%{prj.name}")

	files
	{
		"public/**.h",
        "public/**.hpp",
		"public/**.cpp"
	}

    includedirs { "public/" }

	filter "system:windows"
        systemversion "latest"      

        removefiles {
            "public/libbacktrace/posix.cpp",
            "public/libbacktrace/mmapio.cpp",
            "public/libbacktrace/macho.cpp",
            "public/libbacktrace/fileline.cpp",
            "public/libbacktrace/elf.cpp"
        }

    filter "system:linux"
        systemversion "latest"
        pic "On"
        debugformat "Dwarf-4"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        conformancemode "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"
        conformancemode "On"

        defines {
            "TRACY_ENABLE",
            "TRACY_ON_DEMAND"
        }

    filter "configurations:Distribution"
        runtime "Release"
        optimize "Full"
        conformancemode "On"

        defines {
            "TRACY_ENABLE",
            "TRACY_ON_DEMAND"
        }