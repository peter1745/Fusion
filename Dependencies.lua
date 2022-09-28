VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDirs = {}
IncludeDirs["GLFW"] = "%{wks.location}/ThirdParty/GLFW/include"
IncludeDirs["glm"] = "%{wks.location}/ThirdParty/glm"
IncludeDirs["spdlog"] = "%{wks.location}/ThirdParty/spdlog/include"
IncludeDirs["glad"] = "%{wks.location}/ThirdParty/glad/include"
IncludeDirs["ImGui"] = "%{wks.location}/ThirdParty/ImGui"
IncludeDirs["EnTT"] = "%{wks.location}/ThirdParty/EnTT/include"
IncludeDirs["stb_image"] = "%{wks.location}/ThirdParty/stb_image/include"
IncludeDirs["TinyGLTF"] = "%{wks.location}/ThirdParty/TinyGLTF/include"
