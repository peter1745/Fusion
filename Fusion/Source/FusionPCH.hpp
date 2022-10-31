#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <array>
#include <fstream>
#include <sstream>
#include <filesystem>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Fusion/Core/Core.hpp"

#if defined(FUSION_PLATFORM_WINDOWS)
	#include <Windows.h>
	#include <d3d11.h>
#endif
