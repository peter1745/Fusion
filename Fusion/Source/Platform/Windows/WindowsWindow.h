#pragma once

#include "Fusion/Core/Window.h"

#include <GLFW/glfw3.h>

namespace Fusion {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowSpecification& specification);
		virtual ~WindowsWindow();

		virtual bool Init() override;

		virtual void ProcessEvents() override;
		virtual bool ShouldClose() const override;
		virtual uint32_t GetWidth() const override { return m_Specification.Width; }
		virtual uint32_t GetHeight() const override { return m_Specification.Height; }
		virtual void* GetNativeWindow() const override { return m_NativeWindow; }

	private:
		GLFWwindow* m_NativeWindow = nullptr;
		WindowSpecification m_Specification;
	};

}
