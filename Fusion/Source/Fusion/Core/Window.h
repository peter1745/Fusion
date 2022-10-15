#pragma once

#include "Core.h"
#include "Fusion/Events/Event.h"

#include <string>

namespace Fusion {

	struct WindowSpecification
	{
#ifdef _UNICODE
		std::wstring Title;
#else
		std::string Title;
#endif
		uint32_t Width;
		uint32_t Height;
	};

	class Window
	{
	public:
		using EventCallbackFunc = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void SetEventCallback(const EventCallbackFunc& InCallbackFunc) = 0;

		virtual void* GetWindowHandle() const = 0;
		virtual bool ShouldClose() const = 0;

	private:
		virtual void Init() = 0;
		virtual void ProcessEvents() = 0;

	private:
		static Unique<Window> Create(const WindowSpecification& InSpecification);

	private:
		friend class Application;
	};

}
