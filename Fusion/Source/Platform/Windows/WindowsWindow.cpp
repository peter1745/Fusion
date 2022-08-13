#include "FusionPCH.h"
#include "WindowsWindow.h"
#include "Platform/Vulkan/VulkanContext.h"

#include <iostream>

namespace Fusion {

	std::unique_ptr<VulkanContext> s_VulkanContext;

	WindowsWindow::WindowsWindow(const WindowProperties& properties)
		: m_Properties(properties)
	{
		if (!glfwInit())
			__debugbreak();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		m_NativeWindow = glfwCreateWindow(int(properties.Width), int(properties.Height), properties.Title.data(), NULL, NULL);

		if (!m_NativeWindow)
		{
			__debugbreak();
			glfwTerminate();
			return;
		}

		s_VulkanContext = std::make_unique<VulkanContext>(this);
	}

	WindowsWindow::~WindowsWindow()
	{
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
	}

	bool WindowsWindow::ShouldClose() const
	{
		return glfwWindowShouldClose(m_NativeWindow);
	}

}

