#include "FusionPCH.h"
#include "WorldCamera.h"

namespace Fusion {

	void WorldCamera::SetViewportSize(uint32_t InWidth, uint32_t InHeight)
	{
		SetProjectionMatrix(glm::perspectiveRH_ZO(glm::radians(m_VerticalFOV), float(InHeight) / float(InWidth), m_NearPlane, m_FarPlane));
	}

}
