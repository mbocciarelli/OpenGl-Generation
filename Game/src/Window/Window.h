#pragma once
#include <functional>
#include <memory>
#include <string>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "../Event/Event.h"

enum KeyCode;
enum MouseCode;

using EventCallbackFn = std::function<void(Event&)>;

struct WindowData
{
	WindowData(const std::string& title, int width, int height) : title(title), width(width), height(height) {}

	std::string title;
	int width;
	int height;
	bool vSync = true;

	EventCallbackFn eventCallback;

};

class Window
{
public:
	Window(int width, int height, const std::string& title);
	explicit Window(const WindowData& data);
	~Window();

	static std::unique_ptr<Window> Create(const WindowData& data) 
	{
		return std::make_unique<Window>(data);
	}

private:
	void Init();

public:
	void OnUpdate() const;

	void SetVSync(bool enabled);
	bool IsKeyPressed(KeyCode key);
	bool IsMouseButtonPressed(MouseCode button);
	glm::vec2 GetMousePosition();
	float GetMouseX();
	float GetMouseY();

	void SetEventCallback(const EventCallbackFn& callback) { m_data.eventCallback = callback; }

	[[nodiscard]] int GetWidth() const { return m_data.width; }
	[[nodiscard]] int GetHeight() const { return m_data.height; }

	[[nodiscard]] GLFWwindow* GetNativeWindow() const { return m_window; }

private:
	GLFWwindow* m_window;
	WindowData m_data;
};
