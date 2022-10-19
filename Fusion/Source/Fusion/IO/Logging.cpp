#include "FusionPCH.h"
#include "Logging.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#ifdef FUSION_PLATFORM_WINDOWS
	#include <stdio.h>
	#include <fcntl.h>
	#include <io.h>
	#include <iostream>
	#include <fstream>
#endif

namespace Fusion {

	void Logging::Initalize()
	{
		if (!std::filesystem::exists("Logs"))
			std::filesystem::create_directories("Logs");

		std::vector<spdlog::sink_ptr> FusionSinks = {
			std::make_shared<spdlog::sinks::basic_file_sink_mt>("Logs/Fusion.log", true),
			std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
		};

		std::vector<spdlog::sink_ptr> ClientSinks = {
			std::make_shared<spdlog::sinks::basic_file_sink_mt>("Logs/Client.log", true),
			std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
		};

		FusionSinks[0]->set_pattern("[%T] [%l] %n: %v");
		ClientSinks[0]->set_pattern("[%T] [%l] %n: %v");

		FusionSinks[1]->set_pattern("%^[%T] %n: %v%$");
		ClientSinks[1]->set_pattern("%^[%T] %n: %v%$");

		s_FusionLogger = std::make_shared<spdlog::logger>("Fusion", FusionSinks.begin(), FusionSinks.end());
		s_FusionLogger->set_level(spdlog::level::trace);

		s_ClientLogger = std::make_shared<spdlog::logger>("Client", ClientSinks.begin(), ClientSinks.end());
		s_ClientLogger->set_level(spdlog::level::trace);

		FUSION_CORE_INFO("Initialized Logging System");
	}

	void Logging::Shutdown()
	{
		s_FusionLogger.reset();
		s_ClientLogger.reset();
	}

	std::shared_ptr<spdlog::logger> Logging::s_FusionLogger;
	std::shared_ptr<spdlog::logger> Logging::s_ClientLogger;

}
