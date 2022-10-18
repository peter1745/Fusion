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

#ifdef FUSION_PLATFORM_WINDOWS
	void Logging::CreateWindowsConsole()
	{
		AllocConsole();

		int ConsoleHandle;
		intptr_t StdHandle;
		CONSOLE_SCREEN_BUFFER_INFO ConsoleInfo;
		FILE* StreamHandle;

		// allocate a console for this app
		AllocConsole();

		// set the screen buffer to be big enough to let us scroll text
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ConsoleInfo);
		ConsoleInfo.dwSize.Y = 500;
		SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), ConsoleInfo.dwSize);

		// redirect unbuffered STDOUT to the console
		StdHandle = reinterpret_cast<intptr_t>(GetStdHandle(STD_OUTPUT_HANDLE));
		ConsoleHandle = _open_osfhandle(StdHandle, _O_TEXT);
		StreamHandle = _fdopen(ConsoleHandle, "w");
		*stdout = *StreamHandle;
		setvbuf(stdout, NULL, _IONBF, 0);

		// redirect unbuffered STDIN to the console
		StdHandle = reinterpret_cast<intptr_t>(GetStdHandle(STD_INPUT_HANDLE));
		ConsoleHandle = _open_osfhandle(StdHandle, _O_TEXT);
		StreamHandle = _fdopen(ConsoleHandle, "r");
		*stdin = *StreamHandle;
		setvbuf(stdin, NULL, _IONBF, 0);

		// redirect unbuffered STDERR to the console
		StdHandle = reinterpret_cast<intptr_t>(GetStdHandle(STD_ERROR_HANDLE));
		ConsoleHandle = _open_osfhandle(StdHandle, _O_TEXT);
		StreamHandle = _fdopen(ConsoleHandle, "w");
		*stderr = *StreamHandle;
		setvbuf(stderr, NULL, _IONBF, 0);

		// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
		// point to console as well
		std::ios::sync_with_stdio();
	}
#endif

	std::shared_ptr<spdlog::logger> Logging::s_FusionLogger;
	std::shared_ptr<spdlog::logger> Logging::s_ClientLogger;

}
