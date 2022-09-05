#include "FusionPCH.h"
#include "WindowsWindow.h"

namespace Fusion {

	WindowsWindow::WindowsWindow(const WindowSpecification& InSpecification)
		: m_Specification(InSpecification)
	{
	}

	WindowsWindow::~WindowsWindow()
	{
	}

	void WindowsWindow::Init()
	{
		FUSION_CORE_VERIFY(glfwInit(), "Failed to initialize GLFW!");
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
		m_NativeWindow = glfwCreateWindow(int(m_Specification.Width), int(m_Specification.Height), m_Specification.Title.data(), NULL, NULL);
		FUSION_CORE_VERIFY(m_NativeWindow, "Failed to create GLFW window!");
	}

	void WindowsWindow::ProcessEvents()
	{
		glfwSwapBuffers(m_NativeWindow);
		glfwPollEvents();
	}

	bool WindowsWindow::ShouldClose() const
	{
		return glfwWindowShouldClose(m_NativeWindow);
	}

}

