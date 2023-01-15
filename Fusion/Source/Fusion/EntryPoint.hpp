#pragma once

extern Fusion::Application* Fusion::CreateApplication(int ArgC, char** ArgV);

namespace Fusion {

	int Main(int ArgC, char** ArgV)
	{
		LogSystem::Initalize();
		Application* app = CreateApplication(ArgC, ArgV);
		app->Run();
		delete app;
		LogSystem::Shutdown();
		return 0;
	}

}

#if defined(FUSION_PLATFORM_WINDOWS) && defined(FUSION_DIST)
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
int main(int argc, char** argv)
{
	return Fusion::Main(argc, argv);
}
#endif
