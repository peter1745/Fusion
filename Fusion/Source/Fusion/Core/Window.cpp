#include "FusionPCH.h"
#include "Window.h"
#include "Fusion/IO/Keyboard.h"
#include "Fusion/IO/GLFWKeyMappings.h"

#include "Platform/Windows/WindowsWindow.h"

#include <GLFW/glfw3.h>

namespace Fusion {

	/*void Window::Init()
	{
		FUSION_CORE_VERIFY(glfwInit(), "Failed to initialize GLFW!");
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
		m_NativeWindow = glfwCreateWindow(int(m_Specification.Width), int(m_Specification.Height), m_Specification.Title.data(), NULL, NULL);
		FUSION_CORE_VERIFY(m_NativeWindow, "Failed to create GLFW window!");

		glfwSetKeyCallback(m_NativeWindow, [](GLFWwindow* InNativeWindow, int InKeyCode, int InScanCode, int InAction, int InMods)
		{
			KeyData& Data = Keyboard::Get().GetKeyData(GLFWKeyMappings.at(InKeyCode));
			Data.OldState = Data.CurrentState;

			switch (InAction)
			{
			case GLFW_PRESS:
			{
				Data.CurrentState = EKeyState::Pressed;
				break;
			}
			case GLFW_REPEAT:
			{
				Data.CurrentState = EKeyState::Held;
				break;
			}
			case GLFW_RELEASE:
			{
				Data.CurrentState = EKeyState::Released;
				break;
			}
			}
		});
	}

	void Window::ProcessEvents()
	{
		glfwPollEvents();
	}

	void Window::SwapBuffers()
	{
		glfwSwapBuffers(m_NativeWindow);
	}*/

	Unique<Window> Window::Create(const WindowSpecification& InSpecification)
	{
#ifdef FUSION_PLATFORM_WINDOWS
		return MakeUnique<WindowsWindow>(InSpecification);
#else
		return nullptr;
#endif
	}

	/*bool Window::ShouldClose() const
	{
		return glfwWindowShouldClose(m_NativeWindow);
	}*/

}
