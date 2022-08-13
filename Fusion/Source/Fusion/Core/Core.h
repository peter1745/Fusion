#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	#if defined(_WIN64)
		#define FUSION_PLATFORM_WINDOWS
	#else
		#error Fusion only supports 64-bit Windows platforms!
	#endif
#elif __linux__
	#define FUSION_PLATFORM_LINUX
#endif

#include <cstdint>
