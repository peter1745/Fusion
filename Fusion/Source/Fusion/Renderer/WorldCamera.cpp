#include "FusionPCH.h"
#include "WorldCamera.h"

namespace Fusion {

	void WorldCamera::SetViewportSize(uint32_t InWidth, uint32_t InHeight)
	{
		SetProjectionMatrix(glm::perspectiveFov(glm::radians(m_VerticalFOV), float(InWidth), float(InHeight), m_NearPlane, m_FarPlane));
	}

}
