#pragma once

#include "Shape.hpp"
#include "Fission/Math/AABB.hpp"

#include <glm/glm.hpp>

namespace Fission {

	class BoxShape : public Shape
	{
	public:
		BoxShape(const glm::vec3& InHalfSize)
		    : m_HalfSize(InHalfSize)
		{
			m_BoundingBox = { -InHalfSize, InHalfSize };
		}

		const glm::vec3& GetHalfSize() const { return m_HalfSize; }
		const Math::AABB& GetBoundingBox() const { return m_BoundingBox; }

		constexpr EShapeType GetType() const override { return EShapeType::Box; }

	private:
		glm::vec3 m_HalfSize;
		Math::AABB m_BoundingBox;
	};

}
