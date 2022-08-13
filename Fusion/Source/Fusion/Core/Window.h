#pragma once

#include "Core.h"

#include <string_view>
#include <memory>

namespace Fusion {

	struct WindowProperties
	{
		std::string_view Title;
		uint32_t Width;
		uint32_t Height;
	};

	class Window
	{
	public:
		virtual ~Window() = default;
		virtual void OnUpdate() = 0;
		virtual bool ShouldClose() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual void* GetNativeWindow() const = 0;

	public:
		static std::unique_ptr<Window> Create(const WindowProperties& properties);
	};

}
