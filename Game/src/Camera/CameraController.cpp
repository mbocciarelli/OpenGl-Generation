#include "CameraController.h"

#include <mutex>

#include "../Event/Event.h"
#include "../Event/MouseEvent.h"
#include "../Event/WindowEvent.h"
#include "../Window/Input.h"


CameraController::CameraController(float FOV, float aspectRatio, float near, float far)
	: m_Camera(FOV, aspectRatio, near, far)
{
}

void CameraController::OnUpdate(float ts)
{
	auto translationSpeed = m_CameraTranslationSpeed * ts;
	if (Input::IsKeyPressed(KeyCode::LeftControl))
		translationSpeed *= 10;
	if (Input::IsKeyPressed(KeyCode::A))
	{
		m_Camera.SetPosition(m_Camera.GetPosition() - glm::normalize(glm::cross(m_Camera.GetFront(), m_Camera.GetUp())) * translationSpeed * ts);
	}
	if (Input::IsKeyPressed(KeyCode::D))
	{
		m_Camera.SetPosition(m_Camera.GetPosition() + glm::normalize(glm::cross(m_Camera.GetFront(), m_Camera.GetUp())) * translationSpeed * ts);
	}
	if (Input::IsKeyPressed(KeyCode::W))
	{
		m_Camera.SetPosition(m_Camera.GetPosition() + translationSpeed * ts * m_Camera.GetFront());
	}
	if (Input::IsKeyPressed(KeyCode::S))
	{
		m_Camera.SetPosition(m_Camera.GetPosition() - translationSpeed * ts * m_Camera.GetFront());
	}
    if (Input::IsKeyPressed(KeyCode::Space))
    {
        m_Camera.SetPosition(m_Camera.GetPosition() + translationSpeed * ts * m_Camera.GetUp());
    }
    if (Input::IsKeyPressed(KeyCode::LeftShift))
    {
        m_Camera.SetPosition(m_Camera.GetPosition() - translationSpeed * ts * m_Camera.GetUp());
    }

	if (Input::IsKeyPressed(KeyCode::Q))
		m_Camera.SetYaw(m_Camera.GetYaw() + m_CameraRotationSpeed * ts);
	if (Input::IsKeyPressed(KeyCode::E))
		m_Camera.SetYaw(m_Camera.GetYaw() - m_CameraRotationSpeed * ts);

}

void CameraController::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<MouseScrolledEvent>([this](MouseScrolledEvent& ev) { return CameraController::OnMouseScrolled(ev); });
	dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& ev) { return CameraController::OnWindowResized(ev); });;
	dispatcher.Dispatch<MouseMovedEvent>([this](MouseMovedEvent& ev) { return CameraController::OnMouseMouved(ev); });;
}

void CameraController::OnResize(float width, float height)
{
	m_Camera.SetViewportSize(width, height);
}

bool CameraController::OnMouseMouved(MouseMovedEvent& e)
{
	static float lastX = 800.0f / 2.0f;
	static float lastY = 600.0f / 2.0f;

	static std::once_flag flag;
	std::call_once(flag, [&]()
		{
			lastX = e.GetX();
			lastY = e.GetY();
		});

	float xoffset = e.GetX() - lastX;
	float yoffset = lastY - e.GetY();

	lastX = e.GetX();
	lastY = e.GetY();

	if (Input::IsMouseButtonPressed(ButtonLeft)) {

		float sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		m_Camera.SetYaw(m_Camera.GetYaw() + xoffset);
		m_Camera.SetPitch(m_Camera.GetPitch() + yoffset);
	}

	// could change translation speed here
	return false;
}

bool CameraController::OnMouseScrolled(MouseScrolledEvent& e)
{
	auto fov = m_Camera.GetFOV();
	fov -= e.GetYOffset();
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
	m_Camera.SetFOV(fov);
	// could change translation speed here
	return false;
}

bool CameraController::OnWindowResized(WindowResizeEvent& e)
{
	OnResize(static_cast<float>(e.GetWidth()), static_cast<float>(e.GetHeight()));
	return false;
}