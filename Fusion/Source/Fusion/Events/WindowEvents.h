#pragma once

#include "Event.h"

namespace Fusion {

	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(uint32_t InWidth, uint32_t InHeight)
			: m_Width(InWidth), m_Height(InHeight) {}

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	private:
		uint32_t m_Width;
		uint32_t m_Height;
	};

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() = default;

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

}
