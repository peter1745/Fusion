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

#include <glm/gtc/type_ptr.hpp>

#include "Fusion/Core/Core.h"

#if defined(FUSION_PLATFORM_WINDOWS)
	#include <Windows.h>
	#include <d3d11.h>
	#include <dxgi.h>
#endif
