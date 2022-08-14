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
#include <memory>

#include "Logging.h"
#include "Assert.h"

namespace Fusion {

	using Byte = uint8_t;

	template<typename T>
	using Shared = std::shared_ptr<T>;
	template<typename T, typename... TArgs>
	Shared<T> MakeShared(TArgs&&... InArgs) { return std::make_shared<T>(std::forward<TArgs>(InArgs)...); }


	template<typename T>
	using Unique = std::unique_ptr<T>;
	template<typename T, typename... TArgs>
	Unique<T> MakeUnique(TArgs&&... InArgs) { return std::make_unique<T>(std::forward<TArgs>(InArgs)...); }

}
