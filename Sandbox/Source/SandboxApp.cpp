#include "Fusion/Core/Window.h"

#include <memory>

int main(int argc, char** argv)
{
	Fusion::WindowProperties windowProps;
	windowProps.Title = "Fusion Sandbox App";
	windowProps.Width = 1920;
	windowProps.Height = 1080;
	std::unique_ptr<Fusion::Window> window = Fusion::Window::Create(windowProps);

	while (!window->ShouldClose())
	{
		window->OnUpdate();
	}

	return 0;
}