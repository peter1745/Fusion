include "Dependencies.lua"

workspace "Fusion"
    architecture "x86_64"
    startproject "Sandbox"

    configurations { "Debug", "Release", "Distribution" }

    flags { "MultiProcessorCompile" }

    toolset "clang"

BuildDir = "%{wks.location}/Build/%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"
IntermediatesDir = "%{wks.location}/Build/Intermediates/%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

group "ThirdParty"
include "ThirdParty/GLFW"
include "ThirdParty/glad"
include "ThirdParty/ImGui/ImGui"
group ""

include "Fusion"
include "FusionEditor"
include "Sandbox"
