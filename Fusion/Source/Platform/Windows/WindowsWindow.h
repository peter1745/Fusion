#pragma once

#include "Fusion/Core/Window.h"

#include <GLFW/glfw3.h>

namespace Fusion {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProperties& properties);
		virtual ~WindowsWindow();

		virtual void OnUpdate() override;
		virtual bool ShouldClose() const override;
		virtual uint32_t GetWidth() const override { return m_Properties.Width; }
		virtual uint32_t GetHeight() const override { return m_Properties.Height; }
		virtual void* GetNativeWindow() const override { return m_NativeWindow; }

	private:
		GLFWwindow* m_NativeWindow = nullptr;
		WindowProperties m_Properties;
	};

}
