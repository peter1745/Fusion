#pragma once

#include "Fusion/IO/Logging.hpp"

#if defined(FUSION_PLATFORM_WINDOWS)
#define FUSION_DEBUG_BREAK __debugbreak()
#else
#include <signal.h>
#define FUSION_DEBUG_BREAK raise(SIGABRT)
#endif

#define FUSION_ENABLE_VERIFY 1

#if FUSION_ENABLE_VERIFY

#ifdef FUSION_PLATFORM_WINDOWS
	#define FUSION_CORE_VERIFY_MESSAGE_INTERNAL(...) ::Fusion::Logging::PrintVerifyMessage(true, "Verify Failed" __VA_OPT__(,) __VA_ARGS__)
	#define FUSION_CLIENT_VERIFY_MESSAGE_INTERNAL(...) ::Fusion::Logging::PrintVerifyMessage(false, "Verify Failed" __VA_OPT__(,) __VA_ARGS__)
#else
	#define FUSION_CORE_VERIFY_MESSAGE_INTERNAL(...) ::Fusion::Logging::PrintVerifyMessage(true, "Verify Failed" __VA_OPT__(,) __VA_ARGS__)
	#define FUSION_CLIENT_VERIFY_MESSAGE_INTERNAL(...) ::Fusion::Logging::PrintVerifyMessage(false, "Verify Failed" __VA_OPT__(,) __VA_ARGS__)
#endif

	#define FUSION_CORE_VERIFY(condition, ...) { if (!(condition)) { FUSION_CORE_VERIFY_MESSAGE_INTERNAL(__VA_ARGS__); FUSION_DEBUG_BREAK; } }
	#define FUSION_CLIENT_VERIFY(condition, ...) { if (!(condition)) { FUSION_CLIENT_VERIFY_MESSAGE_INTERNAL(__VA_ARGS__); FUSION_DEBUG_BREAK; } }
#else
	#define FUSION_CORE_VERIFY(condition, ...)
	#define FUSION_CLIENT_VERIFY(condition, ...)
#endif
