#pragma once
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>
class Camera
{
public:
	Camera(float FOV, float aspectRatio, float near, float far): m_FOV(FOV), m_aspectRatio(aspectRatio), m_near(near), m_far(far)
	{
		RecalculateProjection();
		RecalculateFront();
		RecalculateView();
	}

	virtual ~Camera() = default;

	void SetViewportSize(float width, float height)
	{
		m_aspectRatio = width / height;
		RecalculateProjection();
	}

	void SetFOV(float fov)
	{
		m_FOV = fov;
		RecalculateProjection();
	}

	void SetNearClip(float nearClip)
	{
		m_near = nearClip;
		RecalculateProjection();
	}

	void SetFarClip(float farClip)
	{
		m_far = farClip;
		RecalculateProjection();
	}

	void SetPosition (const glm::vec3& position)
	{
		m_position = position;
		RecalculateView();
	}

	void SetYaw(const float yaw)
	{
		m_yaw = yaw;
		RecalculateFront();
		RecalculateView();
	}

	void SetPitch(const float pitch)
	{
		m_pitch = pitch;
		if (m_pitch > 89.0f)
			m_pitch = 89.0f;
		else if (m_pitch < -89.0f)
			m_pitch = -89.0f;
		RecalculateFront();
		RecalculateView();
	}

	[[nodiscard]] const glm::mat4& GetProjection() const { return m_projection; }

	[[nodiscard]] const glm::vec3& GetPosition() const { return m_position; }

	[[nodiscard]] const glm::mat4& GetView() const { return m_view; }

	[[nodiscard]] const glm::mat4& GetViewProjection() const { return m_projection * m_view; }

	[[nodiscard]] float GetYaw() const { return m_yaw; }

	[[nodiscard]] float GetPitch() const { return m_pitch; }

	[[nodiscard]] float GetFar() const { return m_far; }

	[[nodiscard]] float GetNear() const { return m_near; }

	[[nodiscard]] float GetFOV() const { return m_FOV; }

	[[nodiscard]] glm::vec3 GetFront() const { return m_front; }

	[[nodiscard]] glm::vec3 GetUp() const { return m_up; }


private:
	void RecalculateProjection()
	{
		m_projection = glm::perspective(glm::radians(m_FOV), m_aspectRatio, m_near, m_far);
	}

	void RecalculateView()
	{
		m_view = glm::lookAt(m_position, m_position + m_front, m_up);
	}

	void RecalculateFront()
	{
		m_front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		m_front.y = sin(glm::radians(m_pitch));
		m_front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		m_front = glm::normalize(m_front);
	}

	float m_FOV = 45.0f;
	float m_aspectRatio = 0.0f;
	float m_near = 0.01f, m_far = 100.0f;

	float m_yaw = -90.f;
	float m_pitch = 0.f;

	glm::mat4 m_projection = glm::mat4(1.0f);
	glm::vec3 m_position = glm::vec3( 0.f, 0.f, 3.f );
	glm::vec3 m_front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 m_view;


};
