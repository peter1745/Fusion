#pragma once

#include "ConvexShape.hpp"

namespace Fission {

	class BoxShape : public ConvexShape
	{
	public:
		BoxShape(const glm::vec3& InHalfSize);

	private:
		glm::vec3 m_HalfSize;
		
	};

}
