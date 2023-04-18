#pragma once

#include "../Application/Application.h"
#include "../Event/KeyEvent.h"

class Input
{
public:
	static bool IsKeyPressed(const KeyCode key) {
		auto* window = Application::Get().GetWindow().GetNativeWindow();
		const auto state = glfwGetKey(window, key);
		return state == GLFW_PRESS;
	}

	static bool IsMouseButtonPressed(const MouseCode button)
	{
		auto* window = Application::Get().GetWindow().GetNativeWindow();
		const auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	static glm::vec2 GetMousePosition()
	{
		auto* window = Application::Get().GetWindow().GetNativeWindow();
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { static_cast<float>(xpos), static_cast<float>(ypos) };
	}

	static float GetMouseX()
	{
		return GetMousePosition().x;
	}

	static float GetMouseY()
	{
		return GetMousePosition().y;
	}
};
