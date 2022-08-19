#include "FusionPCH.h"
#include "WindowsWindow.h"
#include "Platform/Vulkan/VulkanContext.h"

namespace Fusion {

	std::unique_ptr<VulkanContext> s_VulkanContext;

	WindowsWindow::WindowsWindow(const WindowSpecification& InSpecification)
		: m_Specification(InSpecification)
	{
	}

	WindowsWindow::~WindowsWindow()
	{
		s_VulkanContext = nullptr;
	}

	void WindowsWindow::Init()
	{
		FUSION_CORE_VERIFY(glfwInit(), "Failed to initialize GLFW!");

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		m_NativeWindow = glfwCreateWindow(int(m_Specification.Width), int(m_Specification.Height), m_Specification.Title.data(), NULL, NULL);
		FUSION_CORE_VERIFY(m_NativeWindow, "Failed to create GLFW window!");

		s_VulkanContext = std::make_unique<VulkanContext>(this);
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

