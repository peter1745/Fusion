#include "FusionPCH.h"
#include "Window.h"
#include "Fusion/IO/Mouse.h"
#include "Fusion/IO/Keyboard.h"
#include "Fusion/IO/GLFWKeyMappings.h"

#include "Fusion/Events/WindowEvents.h"
#include "Fusion/Events/KeyboardEvents.h"

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
	
	void Window::SetTitle(const std::string& InTitle)
	{
		glfwSetWindowTitle(m_NativeWindow, InTitle.c_str());
		m_WindowData.Specification.Title = InTitle;
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

		glfwSetCursorPosCallback(m_NativeWindow, [](GLFWwindow* InNativeWindow, double InPosX, double InPosY)
		{
			Mouse::Get().m_Position = { static_cast<float>(InPosX), static_cast<float>(InPosY) };
		});

		glfwSetMouseButtonCallback(m_NativeWindow, [](GLFWwindow* InNativeWindow, int InButton, int InAction, int InMods)
		{
			MouseButtonState& ButtonState = Mouse::Get().m_ButtonStates[InButton];
			ButtonState.OldState = ButtonState.CurrentState;
			
			switch (InAction)
			{
			case GLFW_PRESS:
			{
				ButtonState.CurrentState = EButtonState::Pressed;
				break;
			}
			case GLFW_RELEASE:
			{
				ButtonState.CurrentState = EButtonState::Released;
				break;
			}
			}
		});

		glfwSetKeyCallback(m_NativeWindow, [](GLFWwindow* InNativeWindow, int InKey, int InScanCode, int InAction, int InMods)
		{
			WindowData* Data = static_cast<WindowData*>(glfwGetWindowUserPointer(InNativeWindow));

			KeyData& KeyInfo = Keyboard::Get().GetKeyData(GLFWKeyMappings.at(InKey));
			KeyInfo.OldState = KeyInfo.CurrentState;

			switch (InAction)
			{
			case GLFW_PRESS:
			{
				KeyInfo.CurrentState = EButtonState::Pressed;

				KeyPressedEvent KeyEvent(GLFWKeyMappings.at(InKey));
				Data->EventCallback(KeyEvent);
				break;
			}
			case GLFW_REPEAT:
			{
				// NOTE(Peter): This isn't *technically* necessary, since we manually transition pressed keys to the held state
				KeyInfo.CurrentState = EButtonState::Held;
				break;
			}
			case GLFW_RELEASE:
			{
				KeyInfo.CurrentState = EButtonState::Released;

				KeyReleasedEvent KeyEvent(GLFWKeyMappings.at(InKey));
				Data->EventCallback(KeyEvent);
				break;
			}
			}
		});
	}

	void Window::ProcessEvents()
	{
		EMouseVisibility CurrentVisibility = static_cast<EMouseVisibility>(glfwGetInputMode(m_NativeWindow, GLFW_CURSOR) - GLFW_CURSOR_NORMAL);
		if (CurrentVisibility != Mouse::Get().GetVisibility())
			glfwSetInputMode(m_NativeWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL + static_cast<int32_t>(Mouse::Get().GetVisibility()));

		glfwPollEvents();
	}

	bool Window::ShouldClose() const { return glfwWindowShouldClose(m_NativeWindow); }

	void Window::Maximize()
	{
		glfwMaximizeWindow(m_NativeWindow);
	}

}
