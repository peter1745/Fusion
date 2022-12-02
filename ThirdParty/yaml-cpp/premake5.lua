project "yaml-cpp"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "Off"

    targetdir (BuildDir .. "/%{prj.name}")
    objdir (IntermediatesDir .. "/%{prj.name}")

    files { "src/**.cpp", "src/**.h", "include/yaml-cpp/**.h" }
    includedirs { "include/" }

    defines { "YAML_CPP_STATIC_DEFINE" }

    filter "system:windows"
		systemversion "latest"

    filter "system:linux"
		systemversion "latest"
		pic "On"
        debugformat "Dwarf-4"

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
