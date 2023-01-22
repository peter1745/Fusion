#pragma once

#include "Shapes/SphereShape.hpp"
#include "Shapes/BoxShape.hpp"

namespace Fission {

	class CollisionChecker
	{
	public:
		static bool TestSphereSphereCollision(const glm::vec3& InSphereCenter0, const SphereShape* InSphere0, const glm::vec3& InSphereCenter1, const SphereShape* InSphere1);
		static bool TestSphereBoxCollision(const glm::vec3& InSphereCenter, const SphereShape* InSphere, const glm::vec3& InBoxCenter, const BoxShape* InBox);
		static bool TestBoxBoxCollision(const glm::vec3& InBoxCenter0, const BoxShape* InBox0, const glm::vec3& InBoxCenter1, const BoxShape* InBox1);
	};

}
