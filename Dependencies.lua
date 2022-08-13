VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDirs = {}
IncludeDirs["GLFW"] = "%{wks.location}/ThirdParty/GLFW/include"
IncludeDirs["Vulkan"] = "%{VULKAN_SDK}/Include"

LibraryDirs = {}
LibraryDirs["Vulkan"] = "%{VULKAN_SDK}/Lib"

Libraries = {}
Libraries["Vulkan"] = "%{LibraryDirs.Vulkan}/vulkan-1.lib"
