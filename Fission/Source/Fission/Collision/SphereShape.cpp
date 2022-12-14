#include "SphereShape.hpp"

namespace Fission {

	SphereShape::SphereShape(float InRadius)
		: ConvexShape(EShapeType::Sphere), m_Radius(InRadius)
	{
		m_CenterOfMass = glm::vec3(0.0f);
		m_BoundingBox = AABB(glm::vec3(-InRadius), glm::vec3(InRadius));

		CalculateInertiaTensor();
	}

	void SphereShape::CalculateInertiaTensor()
	{
		// 0.4f = 2.0f / 5.0f
		m_InertiaTensor = glm::mat3(0.4f * m_Radius * m_Radius);
	}

	glm::vec3 SphereShape::GetFurthestPoint(const glm::vec3& InLocation, const glm::vec3& InDirection, const glm::quat& InOrientation, float InBias) const
	{
		return InLocation + InDirection * (m_Radius + InBias);
	}

}
