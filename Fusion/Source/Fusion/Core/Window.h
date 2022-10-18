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

		virtual uint32_t GetWidth() const { return m_WindowData.Specification.Width; }
		virtual uint32_t GetHeight() const { return m_WindowData.Specification.Height; }

		virtual void SetEventCallback(const EventCallbackFunc& InCallbackFunc)
		{
			m_WindowData.EventCallback = InCallbackFunc;
		}

		virtual void* GetWindowHandle() const { return static_cast<void*>(m_NativeWindow); }
		virtual bool ShouldClose() const;

	private:
		virtual void Init();
		virtual void ProcessEvents();

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
