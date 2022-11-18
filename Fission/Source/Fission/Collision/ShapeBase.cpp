#include "ShapeBase.hpp"

namespace Fission {

	AABB ShapeBase::GetBoundingBox(const glm::vec3& InLocation, const glm::quat& InOrientation)
	{
		AABB BoundingBox = m_BoundingBox;
		BoundingBox.Offset(InLocation);
		return BoundingBox;
	}

}
