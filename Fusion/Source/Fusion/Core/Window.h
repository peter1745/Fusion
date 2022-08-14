#pragma once

#include "Core.h"

#include <string>

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
		virtual ~Window() = default;

		virtual bool ShouldClose() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual void* GetNativeWindow() const = 0;

	public:
		static Unique<Window> Create(const WindowSpecification& specification);

	private:
		virtual bool Init() = 0;
		virtual void ProcessEvents() = 0;

		friend class Application;
	};

}
