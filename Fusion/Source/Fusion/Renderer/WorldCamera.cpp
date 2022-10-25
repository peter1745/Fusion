#include "FusionPCH.hpp"
#include "WorldCamera.hpp"

namespace Fusion {

	void WorldCamera::RecalculateProjectionMatrix()
	{
		SetProjectionMatrix(glm::perspectiveRH_ZO(glm::radians(m_VerticalFOV), float(m_Width) / float(m_Height), m_NearPlane, m_FarPlane));
	}

}
