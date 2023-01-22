#pragma once

#include <glm/glm.hpp>

namespace Fission::Math {

	struct AABB
	{
		glm::vec3 MinBound;
		glm::vec3 MaxBound;

		bool IsPointInside(const glm::vec3& InPoint) const;
		bool IsAABBInside(const AABB& InOther) const;

		void Offset(const glm::vec3& InOffset);
	};

}
