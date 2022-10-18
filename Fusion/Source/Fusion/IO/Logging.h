#pragma once

#include "Fusion/Core/Core.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include "CustomLogFormatters.h"

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

#ifdef FUSION_PLATFORM_WINDOWS
		static void CreateWindowsConsole();
#endif

		template<typename... TArgs>
		static void PrintMessage(bool IsFusionLogger, Level InLevel, TArgs&&... InArgs);

		template<typename... TArgs>
		static void PrintVerifyMessage(bool IsFusionLogger, std::string_view InPrefix, TArgs&&... InArgs);

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
	void Logging::PrintMessage(bool IsFusionLogger, Level InLevel, TArgs&&... InArgs)
	{
		auto LoggerPtr = IsFusionLogger ? GetFusionLogger() : GetClientLogger();
		switch (InLevel)
		{
		case Logging::Level::Trace:
			LoggerPtr->trace("{0}", fmt::format(std::forward<TArgs>(InArgs)...));
			break;
		case Logging::Level::Info:
			LoggerPtr->info("{0}", fmt::format(std::forward<TArgs>(InArgs)...));
			break;
		case Logging::Level::Warn:
			LoggerPtr->warn("{0}", fmt::format(std::forward<TArgs>(InArgs)...));
			break;
		case Logging::Level::Error:
			LoggerPtr->error("{0}", fmt::format(std::forward<TArgs>(InArgs)...));
			break;
		case Logging::Level::Fatal:
			LoggerPtr->critical("{0}", fmt::format(std::forward<TArgs>(InArgs)...));
			break;
		}
	}

	template<typename... TArgs>
	void Logging::PrintVerifyMessage(bool IsFusionLogger, std::string_view InPrefix, TArgs&&... InArgs)
	{
		auto LoggerPtr = IsFusionLogger ? GetFusionLogger() : GetClientLogger();
		LoggerPtr->critical("{0}: {1}", InPrefix, fmt::format(std::forward<TArgs>(InArgs)...));
	}

	template<>
	inline void Logging::PrintVerifyMessage(bool IsFusionLogger, std::string_view InPrefix)
	{
		auto LoggerPtr = IsFusionLogger ? GetFusionLogger() : GetClientLogger();
		LoggerPtr->critical("{0}", InPrefix);
	}

}
