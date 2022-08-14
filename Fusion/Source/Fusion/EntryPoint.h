#pragma once

extern Fusion::Application* Fusion::CreateApplication(int ArgC, char** ArgV);

namespace Fusion {

	int Main(int ArgC, char** ArgV)
	{
		Fusion::Application* app = Fusion::CreateApplication(ArgC, ArgV);
		app->Run();
		delete app;
		return 0;
	}

}

#if defined(FUSION_DISTRIBUTION) && defined(FUSION_PLATFORM_WINDOWS)
	int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hInstancePrev, PSTR cmdLine, int cmdShow)
	{
		return Fusion::Main(__argc, __argv);
	}
#else
	int main(int argc, char** argv)
	{
		return Fusion::Main(argc, argv);
	}
#endif
