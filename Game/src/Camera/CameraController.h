#pragma once
#include "Camera.h"

class MouseMovedEvent;
class WindowResizeEvent;
class MouseScrolledEvent;
class Event;

class CameraController
{
public:
	CameraController(float FOV, float aspectRatio, float near, float far);

	void OnUpdate(float ts);
	void OnEvent(Event& e);

	void OnResize(float width, float height);

	bool OnMouseMouved(MouseMovedEvent& e);

	[[nodiscard]] const Camera& GetCamera() const { return m_Camera; }

private:
	bool OnMouseScrolled(MouseScrolledEvent& e);
	bool OnWindowResized(WindowResizeEvent& e);

	Camera m_Camera;

	float m_CameraTranslationSpeed = 0.1f, m_CameraRotationSpeed = 1.f;
};
