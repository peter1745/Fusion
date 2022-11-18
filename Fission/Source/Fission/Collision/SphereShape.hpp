#pragma once

#include "ShapeBase.hpp"

namespace Fission {

	class SphereShape : public ShapeBase
	{
	public:
		SphereShape(float InRadius);

		float GetRadius() const { return m_Radius; }

		virtual void CalculateInertiaTensor() override;

	private:
		float m_Radius;
	};

}
