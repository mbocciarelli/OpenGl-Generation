#pragma once
#include <sstream>

#include "Event.h"
#include "../Window/Window.h"

enum MouseCode
{
	// From glfw3.h
	Button0 = 0,
	Button1 = 1,
	Button2 = 2,
	Button3 = 3,
	Button4 = 4,
	Button5 = 5,
	Button6 = 6,
	Button7 = 7,

	ButtonLast = Button7,
	ButtonLeft = Button0,
	ButtonRight = Button1,
	ButtonMiddle = Button2
};

class MouseMovedEvent : public Event
{
public:
	MouseMovedEvent(const float x, const float y) : m_mouseX(x), m_mouseY(y) {}

	float GetX() const { return m_mouseX; }
	float GetY() const { return m_mouseY; }

	std::string ToString() const override
	{
		std::stringstream ss;
		ss << "MouseMovedEvent: " << m_mouseX << ", " << m_mouseY;
		return ss.str();
	}

	EVENT_CLASS_TYPE(MouseMoved)
	EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

private:
	float m_mouseX, m_mouseY;
};

class MouseScrolledEvent : public Event
{
public:
	MouseScrolledEvent(const float xOffset, const float yOffset)
		: m_xOffset(xOffset), m_yOffset(yOffset) {}

	float GetXOffset() const { return m_xOffset; }
	float GetYOffset() const { return m_yOffset; }

	std::string ToString() const override
	{
		std::stringstream ss;
		ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();
		return ss.str();
	}

	EVENT_CLASS_TYPE(MouseScrolled)
	EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
private:
	float m_xOffset, m_yOffset;
};

class MouseButtonEvent : public Event
{
public:
	MouseCode GetMouseButton() const { return m_button; }

	EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)
protected:
	MouseButtonEvent(const MouseCode button)
		: m_button(button) {}

	MouseCode m_button;
};

class MouseButtonPressedEvent : public MouseButtonEvent
{
public:
	MouseButtonPressedEvent(const MouseCode button)
		: MouseButtonEvent(button) {}

	std::string ToString() const override
	{
		std::stringstream ss;
		ss << "MouseButtonPressedEvent: " << m_button;
		return ss.str();
	}

	EVENT_CLASS_TYPE(MouseButtonPressed)
};

class MouseButtonReleasedEvent : public MouseButtonEvent
{
public:
	MouseButtonReleasedEvent(const MouseCode button)
		: MouseButtonEvent(button) {}

	std::string ToString() const override
	{
		std::stringstream ss;
		ss << "MouseButtonReleasedEvent: " << m_button;
		return ss.str();
	}

	EVENT_CLASS_TYPE(MouseButtonReleased)
};

inline void SetMouseButtonCallbackFunction(GLFWwindow* window, int button, int action, int mods)
{
	WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
	switch (action)
	{
		case GLFW_PRESS:
		{
			MouseButtonPressedEvent event((MouseCode)button);
			data.eventCallback(event);
			break;
		}
		case GLFW_RELEASE:
		{
			MouseButtonReleasedEvent event((MouseCode)button);
			data.eventCallback(event);
			break;
		}
	}
}

inline void SetCursorPosCallbackFunction(GLFWwindow* window, double xpos, double ypos)
{
	WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
	MouseMovedEvent event((float)xpos, (float)ypos);
	data.eventCallback(event);
}

inline void SetScrollCallbackFunction(GLFWwindow* window, double xoffset, double yoffset)
{
	WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
	MouseScrolledEvent event((float)xoffset, (float)yoffset);
	data.eventCallback(event);
}