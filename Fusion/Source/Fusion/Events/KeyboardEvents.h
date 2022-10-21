#pragma once

#include "Event.h"
#include "Fusion/IO/KeyCodes.h"

namespace Fusion {

	class KeyEvent : public Event
	{
	public:
		KeyEvent(EKeyCode InKeyCode)
			: m_KeyCode(InKeyCode) {}

		EKeyCode GetKeyCode() const { return m_KeyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKeyboard);
	private:
		EKeyCode m_KeyCode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(EKeyCode InKeyCode)
			: KeyEvent(InKeyCode) {}

		EVENT_CLASS_TYPE(KeyPressed)
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(EKeyCode InKeyCode)
			: KeyEvent(InKeyCode) {}

		EVENT_CLASS_TYPE(KeyReleased)
	};

}
