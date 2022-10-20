#pragma once

#include "Core.h"
#include "Fusion/Events/Event.h"

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
		using EventCallbackFunc = std::function<void(Event&)>;

		Window(const WindowSpecification& InSpecification);
		~Window();

		uint32_t GetWidth() const { return m_WindowData.Specification.Width; }
		uint32_t GetHeight() const { return m_WindowData.Specification.Height; }

		void SetEventCallback(const EventCallbackFunc& InCallbackFunc)
		{
			m_WindowData.EventCallback = InCallbackFunc;
		}

		void* GetWindowHandle() const { return static_cast<void*>(m_NativeWindow); }
		bool ShouldClose() const;

		void Maximize();

	private:
		void Init();
		void ProcessEvents();

	private:
		struct WindowData
		{
			WindowSpecification Specification;
			EventCallbackFunc EventCallback = nullptr;
		} m_WindowData;

		GLFWwindow* m_NativeWindow = nullptr;

		friend class Application;
	};

}
