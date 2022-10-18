#include "FusionPCH.h"
#include "Window.h"
#include "Fusion/IO/Keyboard.h"
#include "Fusion/IO/GLFWKeyMappings.h"

#include "Fusion/Events/WindowEvents.h"

#include <GLFW/glfw3.h>

namespace Fusion {

	Window::Window(const WindowSpecification& InSpecification)
	{
		m_WindowData.Specification = InSpecification;
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_NativeWindow);
		glfwTerminate();
	}
	
	void Window::Init()
	{
		FUSION_CORE_VERIFY(glfwInit(), "Failed to initialize GLFW!");

		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		m_NativeWindow = glfwCreateWindow(int(m_WindowData.Specification.Width), int(m_WindowData.Specification.Height), m_WindowData.Specification.Title.c_str(), NULL, NULL);
		FUSION_CORE_VERIFY(m_NativeWindow, "Failed to create GLFW window!");

		glfwSetWindowUserPointer(m_NativeWindow, &m_WindowData);

		glfwSetWindowSizeCallback(m_NativeWindow, [](GLFWwindow* InNativeWindow, int32_t InWidth, int32_t InHeight)
		{
			WindowData* Data = static_cast<WindowData*>(glfwGetWindowUserPointer(InNativeWindow));
			Data->Specification.Width = InWidth;
			Data->Specification.Height = InHeight;

			if (Data->EventCallback)
			{
				WindowResizeEvent ResizeEvent(InWidth, InHeight);
				Data->EventCallback(ResizeEvent);
			}
		});
	}

	void Window::ProcessEvents()
	{
		glfwPollEvents();
	}

	bool Window::ShouldClose() const { return glfwWindowShouldClose(m_NativeWindow); }

}
