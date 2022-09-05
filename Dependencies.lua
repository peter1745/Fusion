VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDirs = {}
IncludeDirs["GLFW"] = "%{wks.location}/ThirdParty/GLFW/include"
IncludeDirs["glm"] = "%{wks.location}/ThirdParty/glm"
IncludeDirs["spdlog"] = "%{wks.location}/ThirdParty/spdlog/include"
IncludeDirs["glad"] = "%{wks.location}/ThirdParty/glad/include"
