#include "ViewportCamera.hpp"
#include "Fusion/Core/Application.hpp"
#include "Fusion/IO/Keyboard.hpp"
#include "Fusion/IO/Mouse.hpp"

namespace FusionEditor {

	using namespace Fusion;
	
	// NOTE(Peter): Viewport camera shamelessly stolen from Hazel. Thanks Yan and Karim ;)

	// NOTE(Peter): Stole glms perspective function in order to invert height
	static glm::mat4 PerspectiveFOV(float fov, float width, float height, float zNear, float zFar)
	{
		float rad = fov;
		float h = glm::cos(0.5f * rad) / glm::sin(0.5f * rad);
		float w = h * height / width;

		glm::mat4 Result(0);
		Result[0][0] = w;
		Result[1][1] = -h;
		Result[2][2] = zFar / (zNear - zFar);
		Result[2][3] = -1.0f;
		Result[3][2] = -(zFar * zNear) / (zFar - zNear);
		return Result;
	}

	ViewportCamera::ViewportCamera(uint32_t InWidth, uint32_t InHeight)
	{
		SetViewportSize(InWidth, InHeight);

		constexpr glm::vec3 InitialPosition = { 0, 0, 10 };
		m_Distance = glm::distance(InitialPosition, m_FocalPoint);

		m_Location = m_FocalPoint - GetForwardDirection() * m_Distance + m_LocationDelta;
		const glm::quat Orientation = GetOrientation();
		m_Direction = glm::eulerAngles(Orientation) * (180.0f / glm::pi<float>());
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Location) * glm::toMat4(Orientation);
		m_ViewMatrix = glm::inverse(m_ViewMatrix);
	}

	void ViewportCamera::SetViewportSize(uint32_t InWidth, uint32_t InHeight)
	{
		SetProjectionMatrix(PerspectiveFOV(glm::radians(m_VerticalFOV), (float)InWidth, (float)InHeight, 0.01f, 1000.0f));
	}

	void ViewportCamera::OnUpdate(float InDeltaTime)
	{
		if (!m_IsActive)
		{
			Mouse::Get().SetVisibility(EMouseVisibility::Visible);
			return;
		}

		const glm::vec2& MousePosition = Mouse::Get().GetPosition();
		const glm::vec2 MouseDelta = (MousePosition - m_InitialMousePosition) * 0.002f;

		if (Mouse::Get().IsButtonHeld(EMouseButton::Right))
		{
			Mouse::Get().SetVisibility(EMouseVisibility::Locked);

			const float YawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
			const float CameraSpeed = 1.5f;
			const float RotationSpeed = 0.7f;

			if (Keyboard::Get().IsKeyHeld(EKeyCode::Q))
				m_LocationDelta -= InDeltaTime * CameraSpeed * glm::vec3{ 0.f, YawSign, 0.f };
			if (Keyboard::Get().IsKeyHeld(EKeyCode::E))
				m_LocationDelta += InDeltaTime * CameraSpeed * glm::vec3{ 0.f, YawSign, 0.f };
			if (Keyboard::Get().IsKeyHeld(EKeyCode::S))
				m_LocationDelta -= InDeltaTime * CameraSpeed * m_Direction;
			if (Keyboard::Get().IsKeyHeld(EKeyCode::W))
				m_LocationDelta += InDeltaTime * CameraSpeed * m_Direction;
			if (Keyboard::Get().IsKeyHeld(EKeyCode::A))
				m_LocationDelta -= InDeltaTime * CameraSpeed * m_RightDirection;
			if (Keyboard::Get().IsKeyHeld(EKeyCode::D))
				m_LocationDelta += InDeltaTime * CameraSpeed * m_RightDirection;

			constexpr float MaxRate = 0.12f;
			m_YawDelta += glm::clamp(YawSign * MouseDelta.x * RotationSpeed, -MaxRate, MaxRate);
			m_PitchDelta += glm::clamp(MouseDelta.y * RotationSpeed, -MaxRate, MaxRate);

			m_RightDirection = glm::cross(m_Direction, glm::vec3{ 0.f, YawSign, 0.f });

			m_Direction = glm::rotate(glm::normalize(glm::cross(glm::angleAxis(-m_PitchDelta, m_RightDirection),
				glm::angleAxis(-m_YawDelta, glm::vec3{ 0.f, YawSign, 0.f }))), m_Direction);

			const float distance = glm::distance(m_FocalPoint, m_Location);
			m_FocalPoint = m_Location + GetForwardDirection() * distance;
			m_Distance = distance;
		}
		else
		{
			Mouse::Get().SetVisibility(EMouseVisibility::Visible);
		}

		m_InitialMousePosition = MousePosition;
		m_Location += m_LocationDelta;
		m_Yaw += m_YawDelta;
		m_Pitch += m_PitchDelta;

		UpdateCameraView();
	}

	void ViewportCamera::UpdateCameraView()
	{
		const float YawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;

		// Extra step to handle the problem when the camera direction is the same as the up vector
		const float CosAngle = glm::dot(GetForwardDirection(), GetUpDirection());
		if (CosAngle * YawSign > 0.99f)
			m_PitchDelta = 0.f;

		const glm::vec3 LookDirection = m_Location + GetForwardDirection();
		m_Direction = glm::normalize(LookDirection - m_Location);
		m_Distance = glm::distance(m_Location, m_FocalPoint);
		m_ViewMatrix = glm::lookAt(m_Location, LookDirection, glm::vec3{ 0.f, YawSign, 0.f });

		// Damping
		m_YawDelta *= 0.6f;
		m_PitchDelta *= 0.6f;
		m_LocationDelta *= 0.8f;
	}

}
