#pragma once

#include "Camera.h"

namespace Fusion {

	class WorldCamera : public Camera
	{
	public:
		float GetNearPlane() const { return m_NearPlane; }
		void SetNearPlane(float InNearPlane)
		{
			m_NearPlane = InNearPlane;
			RecalculateProjectionMatrix();
		}

		float GetFarPlane() const { return m_FarPlane; }
		void SetFarPlane(float InFarPlane)
		{
			m_FarPlane = InFarPlane;
			RecalculateProjectionMatrix();
		}

		float GetVerticalFOV() const { return m_VerticalFOV; }
		void SetVerticalFOV(float InVerticalFOV)
		{
			m_VerticalFOV = InVerticalFOV;
			RecalculateProjectionMatrix();
		}

		void SetViewportSize(uint32_t InWidth, uint32_t InHeight)
		{
			m_Width = InWidth;
			m_Height = InHeight;
			RecalculateProjectionMatrix();
		}

		void RecalculateProjectionMatrix();

	private:
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;

		// General Parameters
		float m_NearPlane = 0.1f;
		float m_FarPlane = 1000.0f;

		// Perspective Parameters
		float m_VerticalFOV = 70.0f;

		// Orthographic Parameters
		float m_OrthographicSize = 10.0f;

	};

}
