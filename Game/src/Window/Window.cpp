#include "Window.h"
#include "../Event/WindowEvent.h"
#include "../Event/KeyEvent.h"
#include "../Event/MouseEvent.h"

#include <stdexcept>


Window::Window(int width, int height, const std::string& title) : m_data(title, width, height)
{
    Init();
}

Window::Window(const WindowData& data) : m_data(data)
{
    Init();
}

Window::~Window()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Window::Init()
{
    if (!glfwInit()) {
        throw std::runtime_error("glfw Init failed");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(m_data.width, m_data.height, m_data.title.c_str(), NULL, NULL);

    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("create window failed");
    }

    glfwMakeContextCurrent(m_window);

    SetVSync(m_data.vSync);
    glfwSetWindowUserPointer(m_window, &m_data);

    // Set GLFW callbacks
    glfwSetWindowSizeCallback(m_window, SetWindowSizeCallbackFunction);
    glfwSetWindowCloseCallback(m_window, SetWindowCloseCallbackFunctions);
    glfwSetKeyCallback(m_window, SetKeyCallbackFunction);
    glfwSetMouseButtonCallback(m_window, SetMouseButtonCallbackFunction);
    glfwSetCursorPosCallback(m_window, SetCursorPosCallbackFunction);
    glfwSetScrollCallback(m_window, SetScrollCallbackFunction);

    // fucking lines of hell
    glewExperimental = GL_TRUE;
    if (glewInit())
        throw std::runtime_error("Error init glew");
}

void Window::OnUpdate() const
{
    glfwPollEvents();
    glfwSwapBuffers(m_window);
}

void Window::SetVSync(const bool enabled)
{
    if (enabled) {
        glfwSwapInterval(1);
    }
    else {
        glfwSwapInterval(0);
    }
    m_data.vSync = enabled;
}

bool Window::IsKeyPressed(const KeyCode key)
{
	const auto state = glfwGetKey(m_window, key);
    return state == GLFW_PRESS;
}

bool Window::IsMouseButtonPressed(const MouseCode button)
{
	const auto state = glfwGetMouseButton(m_window, button);
    return state == GLFW_PRESS;
}

glm::vec2 Window::GetMousePosition()
{
    double xpos, ypos;
    glfwGetCursorPos(m_window, &xpos, &ypos);

    return { static_cast<float>(xpos), static_cast<float>(ypos) };
}

float Window::GetMouseX()
{
    return GetMousePosition().x;
}

float Window::GetMouseY()
{
    return GetMousePosition().y;
}