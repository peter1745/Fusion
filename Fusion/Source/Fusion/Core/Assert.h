#pragma once

#if defined(FUSION_PLATFORM_WINDOWS)
#define FUSION_DEBUG_BREAK __debugbreak()
#else
#define FUSION_DEBUG_BREAK
#endif

#define FUSION_ENABLE_VERIFY 1

#if FUSION_ENABLE_VERIFY
	#define FUSION_CORE_VERIFY_MESSAGE_INTERNAL(...) ::Fusion::Logging::PrintVerifyMessage(true, "Verify Failed", __VA_ARGS__)
	#define FUSION_CLIENT_VERIFY_MESSAGE_INTERNAL(...) ::Fusion::Logging::PrintVerifyMessage(false, "Verify Failed", __VA_ARGS__)

	#define FUSION_CORE_VERIFY(condition, ...) { if (!(condition)) { FUSION_CORE_VERIFY_MESSAGE_INTERNAL(__VA_ARGS__); __debugbreak(); } }
	#define FUSION_CLIENT_VERIFY(condition, ...) { if (!(condition)) { FUSION_CLIENT_VERIFY_MESSAGE_INTERNAL(__VA_ARGS__); __debugbreak(); } }
#else
	#define FUSION_CORE_VERIFY(condition, ...)
	#define FUSION_CLIENT_VERIFY(condition, ...)
#endif
