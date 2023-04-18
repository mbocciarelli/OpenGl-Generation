#pragma once
#include <string>
#include <sstream>

enum class EventType
{
	None = 0,
	WindowClose, WindowResize,
	KeyPressed, KeyReleased, KeyTyped,
	MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
};

enum EventCategory
{
	None = 0x0,
	EventCategoryApplication = 0x1,
	EventCategoryInput = 0x2,
	EventCategoryKeyboard = 0x4,
	EventCategoryMouse = 0x8,
	EventCategoryMouseButton = 0x10
};

class Event
{
public:
	virtual ~Event() = default;

	virtual EventType GetEventType() const = 0;
	virtual const char* GetName() const = 0;
	virtual int GetCategoryFlags() const = 0;
	virtual std::string ToString() const { return GetName(); }

	bool IsInCategory(EventCategory category)
	{
		return GetCategoryFlags() & category;
	}

	bool Handled = false;
};

#define EVENT_CLASS_TYPE(type)\
static EventType GetStaticType() { return EventType::type; } \
virtual EventType GetEventType() const override { return GetStaticType(); }\
virtual const char* GetName() const override { return #type; }\

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

class EventDispatcher
{
public:
	explicit EventDispatcher(Event& event)
		: m_Event(event)
	{
	}

	// F will be deduced by the compiler
	template<typename T, typename F>
	bool Dispatch(const F& func)
	{
		if (m_Event.GetEventType() == T::GetStaticType())
		{
			m_Event.Handled |= func(static_cast<T&>(m_Event));
			return true;
		}
		return false;
	}
private:
	Event& m_Event;
};

inline std::ostream& operator<<(std::ostream& os, const Event& e)
{
	return os << e.ToString();
}