#include "FusionPCH.h"
#include "WindowsWindow.h"
#include "Platform/Vulkan/VulkanContext.h"

#include <iostream>

namespace Fusion {

	std::unique_ptr<VulkanContext> s_VulkanContext;

	WindowsWindow::WindowsWindow(const WindowSpecification& specification)
		: m_Specification(specification)
	{
	}

	WindowsWindow::~WindowsWindow()
	{
	}

	bool WindowsWindow::Init()
	{
		if (!glfwInit())
		{
			__debugbreak();
			return false;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		m_NativeWindow = glfwCreateWindow(int(m_Specification.Width), int(m_Specification.Height), m_Specification.Title.data(), NULL, NULL);

		if (!m_NativeWindow)
		{
			__debugbreak();
			glfwTerminate();
			return false;
		}

		//s_VulkanContext = std::make_unique<VulkanContext>(this);
		return true;
	}

	void WindowsWindow::ProcessEvents()
	{
		glfwPollEvents();
	}

	bool WindowsWindow::ShouldClose() const
	{
		return glfwWindowShouldClose(m_NativeWindow);
	}

}

