#pragma once
#include <sstream>
#include "Event.h"
#include "../Window/Window.h"

class WindowResizeEvent : public Event
{
public:
	WindowResizeEvent(int width, int height) : m_width(width), m_height(height) {}

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }

	std::string ToString() const override
	{
		std::stringstream ss;
		ss << "WindowResizeEvent: " << m_width << ", " << m_height;
		return ss.str();
	}

	EVENT_CLASS_TYPE(WindowResize)
	EVENT_CLASS_CATEGORY(EventCategoryApplication)
private:
	int m_width, m_height;
};

class WindowCloseEvent : public Event
{
public:
	WindowCloseEvent() = default;

	EVENT_CLASS_TYPE(WindowClose)
	EVENT_CLASS_CATEGORY(EventCategoryApplication)

};


inline void SetWindowSizeCallbackFunction(GLFWwindow* window, int width, int height)
{
	WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
	data.width = width;
	data.height = height;
	WindowResizeEvent event(width, height);
	data.eventCallback(event);
}

inline void SetWindowCloseCallbackFunctions(GLFWwindow* window)
{
	const WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
	WindowCloseEvent event;
	data.eventCallback(event);
}
