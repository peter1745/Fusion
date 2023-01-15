#include "FusionPCH.hpp"
#include "WorldCamera.hpp"

namespace Fusion {

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

	void WorldCamera::RecalculateProjectionMatrix()
	{
		SetProjectionMatrix(PerspectiveFOV(glm::radians(m_VerticalFOV), float(m_Width), float(m_Height), m_NearPlane, m_FarPlane));
	}

}
