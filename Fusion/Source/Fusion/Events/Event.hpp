#pragma once

#include <string>

namespace Fusion {

	enum class EEventType
	{
		None = 0,
		WindowClose, WindowMinimize, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EEventCategory
	{
		None = 0,
		EventCategoryApplication = (1 << 0),
		EventCategoryInput = (1 << 1),
		EventCategoryKeyboard = (1 << 2),
		EventCategoryMouse = (1 << 3),
		EventCategoryMouseButton = (1 << 4)
	};


#define EVENT_CLASS_TYPE(type) static EEventType GetStaticType() { return EEventType::type; }\
								virtual EEventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	class Event
	{
	public:
		bool Handled = false;

		virtual ~Event() {}
		virtual EEventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int32_t GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EEventCategory InCategory) const { return GetCategoryFlags() & InCategory; }
	};

	class EventDispatcher
	{
	public:
		EventDispatcher(Event& InEvent)
			: m_Event(InEvent) {}

		template<typename TEvent, typename TEventHandler>
		void Dispatch(const TEventHandler& InEventHandler)
		{
			if (m_Event.GetEventType() == TEvent::GetStaticType())
				m_Event.Handled |= InEventHandler(static_cast<TEvent&>(m_Event));
		}

	private:
		Event& m_Event;
	};

}
