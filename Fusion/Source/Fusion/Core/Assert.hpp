#pragma once

#include "Fusion/IO/Logging.hpp"

#if defined(FUSION_PLATFORM_WINDOWS)
	#define FUSION_DEBUG_BREAK __debugbreak()
#else
	#include <signal.h>
	#define FUSION_DEBUG_BREAK raise(SIGABRT)
#endif

#define FUSION_ENABLE_VERIFY 1

namespace Fusion {

#if FUSION_ENABLE_VERIFY

	template<typename... TArgs>
	inline void CoreVerify(bool InCondition, TArgs&&... InArgs)
	{
		if (InCondition)
			return;

		if constexpr (sizeof...(InArgs) > 0)
			LogFatal("Fusion", "Verify Failed: {}", fmt::format(std::forward<TArgs>(InArgs)...));
		else
			LogFatal("Fusion", "Verify Failed");

		FUSION_DEBUG_BREAK;
	}

#else
	template <typename... TArgs>
	inline void CoreVerify(bool InCondition, TArgs&&... InArgs)
	{
	}

#endif

}
