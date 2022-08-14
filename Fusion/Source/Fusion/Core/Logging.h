#pragma once

#include "Core.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Fusion {

	class Logging
	{
	public:
		using Logger = std::shared_ptr<spdlog::logger>;

		enum class Level
		{
			Trace, Info, Warn, Error, Fatal
		};

	public:
		static void Initalize();
		static void Shutdown();

		inline static const Logger& GetFusionLogger() { return s_FusionLogger; }
		inline static const Logger& GetClientLogger() { return s_ClientLogger; }

		template<typename... TArgs>
		static void PrintMessage(bool FusionLogger, Level level, TArgs&&... Args);

		template<typename... TArgs>
		static void PrintVerifyMessage(bool FusionLogger, std::string_view Prefix, TArgs&&... Args);

	private:
		static Logger s_FusionLogger;
		static Logger s_ClientLogger;
	};

}

#define FUSION_CORE_TRACE(...)		::Fusion::Logging::PrintMessage(true, ::Fusion::Logging::Level::Trace, __VA_ARGS__)
#define FUSION_CORE_INFO(...)		::Fusion::Logging::PrintMessage(true, ::Fusion::Logging::Level::Info, __VA_ARGS__)
#define FUSION_CORE_WARN(...)		::Fusion::Logging::PrintMessage(true, ::Fusion::Logging::Level::Warn, __VA_ARGS__)
#define FUSION_CORE_ERROR(...)		::Fusion::Logging::PrintMessage(true, ::Fusion::Logging::Level::Error, __VA_ARGS__)
#define FUSION_CORE_FATAL(...)		::Fusion::Logging::PrintMessage(true, ::Fusion::Logging::Level::Fatal, __VA_ARGS__)

#define FUSION_CLIENT_TRACE(...)	::Fusion::Logging::PrintMessage(false, ::Fusion::Logging::Level::Trace, __VA_ARGS__)
#define FUSION_CLIENT_INFO(...)		::Fusion::Logging::PrintMessage(false, ::Fusion::Logging::Level::Info, __VA_ARGS__)
#define FUSION_CLIENT_WARN(...)		::Fusion::Logging::PrintMessage(false, ::Fusion::Logging::Level::Warn, __VA_ARGS__)
#define FUSION_CLIENT_ERROR(...)	::Fusion::Logging::PrintMessage(false, ::Fusion::Logging::Level::Error, __VA_ARGS__)
#define FUSION_CLIENT_FATAL(...)	::Fusion::Logging::PrintMessage(false, ::Fusion::Logging::Level::Fatal, __VA_ARGS__)

namespace Fusion {


	template<typename... TArgs>
	void Logging::PrintMessage(bool FusionLogger, Level level, TArgs&&... Args)
	{
		auto LoggerPtr = FusionLogger ? GetFusionLogger() : GetClientLogger();
		switch (level)
		{
		case Logging::Level::Trace:
			LoggerPtr->trace("{0}", fmt::format(std::forward<TArgs>(Args)...));
			break;
		case Logging::Level::Info:
			LoggerPtr->info("{0}", fmt::format(std::forward<TArgs>(Args)...));
			break;
		case Logging::Level::Warn:
			LoggerPtr->warn("{0}", fmt::format(std::forward<TArgs>(Args)...));
			break;
		case Logging::Level::Error:
			LoggerPtr->error("{0}", fmt::format(std::forward<TArgs>(Args)...));
			break;
		case Logging::Level::Fatal:
			LoggerPtr->critical("{0}", fmt::format(std::forward<TArgs>(Args)...));
			break;
		}
	}

	template<typename... TArgs>
	void Logging::PrintVerifyMessage(bool FusionLogger, std::string_view Prefix, TArgs&&... Args)
	{
		auto LoggerPtr = FusionLogger ? GetFusionLogger() : GetClientLogger();
		LoggerPtr->critical("{0}: {1}", Prefix, fmt::format(std::forward<TArgs>(Args)...));
	}

	template<>
	inline void Logging::PrintVerifyMessage(bool FusionLogger, std::string_view Prefix)
	{
		auto LoggerPtr = FusionLogger ? GetFusionLogger() : GetClientLogger();
		LoggerPtr->critical("{0}", Prefix);
	}

}
