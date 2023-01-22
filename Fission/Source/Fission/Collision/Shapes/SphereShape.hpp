#pragma once

#include "Shape.hpp"

namespace Fission {

	class SphereShape : public Shape
	{
	public:
		SphereShape(float InRadius)
		    : m_Radius(InRadius) {}

		float GetRadius() const { return m_Radius; }

		constexpr EShapeType GetType() const override { return EShapeType::Sphere; }

	private:
		float m_Radius;
	};

}
