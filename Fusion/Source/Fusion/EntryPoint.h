#pragma once

extern Fusion::Application* Fusion::CreateApplication(int ArgC, char** ArgV);

namespace Fusion {

	int Main(int ArgC, char** ArgV)
	{
		Fusion::Logging::Initalize();
		Fusion::Application* app = Fusion::CreateApplication(ArgC, ArgV);
		app->Run();
		delete app;
		Fusion::Logging::Shutdown();
		return 0;
	}

}

#if defined(FUSION_PLATFORM_WINDOWS)
	#ifdef _UNICODE
		int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hInstancePrev, PWSTR cmdLine, int cmdShow)
		{
			return Fusion::Main(__argc, __argv);
		}
	#else
		int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hInstancePrev, PSTR cmdLine, int cmdShow)
		{
			return Fusion::Main(__argc, __argv);
		}
	#endif
#else
#error Fusion only supports Windows at the moment!
#endif
