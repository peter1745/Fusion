include "Dependencies.lua"

workspace "Fusion"
    architecture "x86_64"
    startproject "Sandbox"

    configurations { "Debug", "Release", "Distribution" }

    flags { "MultiProcessorCompile" }

BuildDir = "%{wks.location}/Build/%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"
IntermediatesDir = "%{wks.location}/Build/Intermediates/%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

group "ThirdParty"
include "ThirdParty/GLFW"
group ""

include "Fusion"
include "Sandbox"
