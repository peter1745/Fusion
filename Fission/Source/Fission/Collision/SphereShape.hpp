#pragma once

#include "ConvexShape.hpp"

namespace Fission {

	class SphereShape : public ConvexShape
	{
	public:
		SphereShape(float InRadius);

		float GetRadius() const { return m_Radius; }

		virtual void CalculateInertiaTensor() override;

		virtual glm::vec3 GetFurthestPoint(const glm::vec3& InLocation, const glm::vec3& InDirection, const glm::quat& InOrientation, float InBias) const override;

	private:
		float m_Radius;
	};

}
