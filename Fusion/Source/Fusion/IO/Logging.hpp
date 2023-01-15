#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include "CustomLogFormatters.hpp"

namespace Fusion {

	enum class ELogLevel { Trace, Info, Warn, Error, Fatal };

	class Logger
	{
	public:
		void Trace(auto&&... InArgs)
		{
			m_Logger->trace("{}", fmt::format(std::forward<decltype(InArgs)>(InArgs)...));
		}

		void Info(auto&&... InArgs)
		{
			m_Logger->info("{}", fmt::format(std::forward<decltype(InArgs)>(InArgs)...));
		}

		void Warn(auto&&... InArgs)
		{
			m_Logger->warn("{}", fmt::format(std::forward<decltype(InArgs)>(InArgs)...));
		}

		void Error(auto&&... InArgs)
		{
			m_Logger->error("{}", fmt::format(std::forward<decltype(InArgs)>(InArgs)...));
		}

		void Fatal(auto&&... InArgs)
		{
			m_Logger->critical("{}", fmt::format(std::forward<decltype(InArgs)>(InArgs)...));
		}

	private:
		std::shared_ptr<spdlog::logger> m_Logger;

	private:
		friend class LogSystem;
	};

	class LogSystem
	{
	public:
		static void Initalize();
		static void Shutdown();

		static Logger* CreateLogger(const std::string& InName, std::string InPattern = "%^[%T] %n: %v%$", ELogLevel InLevel = ELogLevel::Trace);
		static Logger* GetLogger(const std::string& InName);
		static Logger* GetOrCreateLogger(const std::string& InName);
	};

	inline void LogTrace(const std::string& InName, auto&&... InArgs)
	{
		LogSystem::GetOrCreateLogger(InName)->Trace(std::forward<decltype(InArgs)>(InArgs)...);
	}

	inline void LogInfo(const std::string& InName, auto&&... InArgs)
	{
		LogSystem::GetOrCreateLogger(InName)->Info(std::forward<decltype(InArgs)>(InArgs)...);
	}

	inline void LogWarn(const std::string& InName, auto&&... InArgs)
	{
		LogSystem::GetOrCreateLogger(InName)->Warn(std::forward<decltype(InArgs)>(InArgs)...);
	}

	inline void LogError(const std::string& InName, auto&&... InArgs)
	{
		LogSystem::GetOrCreateLogger(InName)->Error(std::forward<decltype(InArgs)>(InArgs)...);
	}

	inline void LogFatal(const std::string& InName, auto&&... InArgs)
	{
		LogSystem::GetOrCreateLogger(InName)->Fatal(std::forward<decltype(InArgs)>(InArgs)...);
	}

}
