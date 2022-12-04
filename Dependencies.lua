VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDirs = {}
IncludeDirs["GLFW"] = "%{wks.location}/ThirdParty/GLFW/include"
IncludeDirs["glm"] = "%{wks.location}/ThirdParty/glm"
IncludeDirs["spdlog"] = "%{wks.location}/ThirdParty/spdlog/include"
IncludeDirs["ImGui"] = "%{wks.location}/ThirdParty/ImGui"
IncludeDirs["EnTT"] = "%{wks.location}/ThirdParty/EnTT/include"
IncludeDirs["stb_image"] = "%{wks.location}/ThirdParty/stb_image/include"
IncludeDirs["TinyGLTF"] = "%{wks.location}/ThirdParty/TinyGLTF/include"
IncludeDirs["yaml_cpp"] = "%{wks.location}/ThirdParty/yaml-cpp/include"
IncludeDirs["NFD_Extended"] = "%{wks.location}/ThirdParty/NFD-Extended/include"
IncludeDirs["DXC"] = "%{wks.location}/ThirdParty/DXC/inc"
IncludeDirs["cpp_sort"] = "%{wks.location}/ThirdParty/cpp-sort/Include"
IncludeDirs["Tracy"] = "%{wks.location}/ThirdParty/tracy/tracy/public"
IncludeDirs["glad"] = "%{wks.location}/ThirdParty/glad/include"
IncludeDirs["shaderc"] = "%{wks.location}/ThirdParty/shaderc/include"
IncludeDirs["SPIRV_Cross"] = "%{wks.location}/ThirdParty/SPIRV-Cross/include"
IncludeDirs["VulkanMemoryAllocator"] = "%{wks.location}/ThirdParty/VulkanMemoryAllocator/include"

LibraryDir = {}
LibraryDir["shaderc"] = "%{wks.location}/ThirdParty/shaderc/lib/%{cfg.buildcfg}"
LibraryDir["SPIRV_Cross"] = "%{wks.location}/ThirdParty/SPIRV-Cross/lib/%{cfg.buildcfg}"
LibraryDir["VulkanMemoryAllocator"] = "%{wks.location}/ThirdParty/VulkanMemoryAllocator/lib/%{cfg.buildcfg}"

