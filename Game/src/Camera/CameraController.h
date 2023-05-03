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

	[[nodiscard]] Camera& GetCamera() { return m_Camera; }

	[[nodiscard]] float GetCameraTranslationSpeed() const { return m_CameraTranslationSpeed; }
	void SetCameraTranslationSpeed(float speed) { m_CameraTranslationSpeed = speed; }

private:
	bool OnMouseScrolled(MouseScrolledEvent& e);
	bool OnWindowResized(WindowResizeEvent& e);

	Camera m_Camera;

	float m_CameraTranslationSpeed = 1000.f, m_CameraRotationSpeed = 2.f;
};
