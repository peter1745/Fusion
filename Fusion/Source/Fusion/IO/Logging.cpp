#include "FusionPCH.hpp"
#include "Logging.hpp"

#include "Fusion/Core/Assert.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Fusion {

	struct LogSystemData
	{
		std::unordered_map<std::string, std::unique_ptr<Logger>> Loggers;
	};

	static LogSystemData* s_Data = nullptr;

	static spdlog::level::level_enum LogLevelToSpdlogLevel(ELogLevel InLevel)
	{
		switch (InLevel)
		{
		case ELogLevel::Trace: return spdlog::level::trace;
		case ELogLevel::Info: return spdlog::level::info;
		case ELogLevel::Warn: return spdlog::level::warn;
		case ELogLevel::Error: return spdlog::level::err;
		case ELogLevel::Fatal: return spdlog::level::critical;
		}

		return spdlog::level::trace;
	}

	void LogSystem::Initalize()
	{
		CoreVerify(s_Data == nullptr);
		s_Data = new LogSystemData();

		if (!std::filesystem::exists("Logs"))
			std::filesystem::create_directories("Logs");

		GetOrCreateLogger("Fusion")->Info("Initialized Logging System.");
	}

	void LogSystem::Shutdown()
	{
		CoreVerify(s_Data != nullptr);
		delete s_Data;
	}

	Logger* LogSystem::CreateLogger(const std::string& InName, std::string InPattern /*= "[%T] [%l] %n: %v"*/, ELogLevel InLevel /*= ELogLevel::Trace*/)
	{
		CoreVerify(s_Data != nullptr);

		auto Sinks = std::array<spdlog::sink_ptr, 2>
		{
			std::make_shared<spdlog::sinks::basic_file_sink_mt>(fmt::format("Logs/{}.log", InName), true),
			std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
		};

		for (auto Sink : Sinks)
			Sink->set_pattern(InPattern);

		s_Data->Loggers[InName] = std::make_unique<Logger>();
		auto& LoggerData = s_Data->Loggers[InName];
		LoggerData->m_Logger = std::make_shared<spdlog::logger>(InName, Sinks.begin(), Sinks.end());
		LoggerData->m_Logger->set_level(LogLevelToSpdlogLevel(InLevel));

		return LoggerData.get();
	}

	Logger* LogSystem::GetLogger(const std::string& InName)
	{
		CoreVerify(s_Data != nullptr);
		return s_Data->Loggers.at(InName).get();
	}

	Logger* LogSystem::GetOrCreateLogger(const std::string& InName)
	{
		CoreVerify(s_Data != nullptr);
		
		if (s_Data->Loggers.find(InName) == s_Data->Loggers.end())
			return CreateLogger(InName);
		else
			return s_Data->Loggers.at(InName).get();
	}

}
