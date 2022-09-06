#pragma once

#include "Core.h"

#include <string>

struct GLFWwindow;

namespace Fusion {

	struct WindowSpecification
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;
	};

	class Window
	{
	public:
		Window(const WindowSpecification& InSpecification);
		~Window();

		bool ShouldClose() const;
		uint32_t GetWidth() const { return m_Specification.Width; }
		uint32_t GetHeight() const { return m_Specification.Height; }
		void* GetNativeWindow() const { return m_NativeWindow; }

	private:
		void Init();
		void ProcessEvents();
		void SwapBuffers();

	private:
		GLFWwindow* m_NativeWindow = nullptr;
		WindowSpecification m_Specification;

	private:
		friend class Application;
	};

}
