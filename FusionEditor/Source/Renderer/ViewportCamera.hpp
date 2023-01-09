#pragma once

#include <Fusion/Renderer/Camera.hpp>

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace FusionEditor {

	// NOTE(Peter): Viewport camera shamelessly stolen from Hazel. Thanks Yan and Karim ;)

	class ViewportCamera : public Fusion::Camera
	{
	public:
		ViewportCamera(uint32_t InWidth, uint32_t InHeight);

		void SetViewportSize(uint32_t InWidth, uint32_t InHeight);

		void OnUpdate(float InDeltaTime);
		void SetActive(bool InActive) { m_IsActive = InActive; }

		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }

	private:
		glm::quat GetOrientation() const { return glm::quat(glm::vec3(-m_Pitch - m_PitchDelta, -m_Yaw - m_YawDelta, 0.0f)); }
		glm::vec3 GetUpDirection() const { return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f)); }
		glm::vec3 GetRightDirection() const { return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f)); }
		glm::vec3 GetForwardDirection() const { return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f)); }

		void UpdateCameraView();

	private:
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		glm::vec3 m_Location = glm::vec3(0.0f);
		glm::vec3 m_LocationDelta = glm::vec3(0.0f);
		glm::vec3 m_Direction;
		glm::vec3 m_FocalPoint = glm::vec3(0.0f, 0.0f, 1.0f);

		glm::vec3 m_RightDirection;

		float m_Distance = 0.0f;
		glm::vec2 m_InitialMousePosition = glm::vec2(0.0f);

		float m_Pitch = 0.0f, m_PitchDelta = 0.0f;
		float m_Yaw = 0.0f, m_YawDelta = 0.0f;

		glm::vec3 m_Rotation = glm::vec3(0.0f);

		float m_VerticalFOV = 70.0f;
		float m_NearPlane = 0.1f;
		float m_FarPlane = 1000.0f;

		bool m_IsActive = false;
	};

}
