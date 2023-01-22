#include "FissionPCH.hpp"
#include "CollisionChecker.hpp"

#include <iostream>
#include <format>

namespace Fission {

	bool CollisionChecker::TestSphereSphereCollision(const glm::vec3& InSphereCenter0, const SphereShape* InSphere0, const glm::vec3& InSphereCenter1, const SphereShape* InSphere1)
	{
		// We use dot(Distance, Distance) < RadiusSum^2 so we don't have to do a sqrt (may or may not make a big difference)
		float RadiusSum = InSphere0->GetRadius() + InSphere1->GetRadius();
		glm::vec3 Distance = InSphereCenter0 - InSphereCenter1;

		float D = glm::dot(Distance, Distance);
		std::cout << std::format("RadiusSum^2={}, D={}", RadiusSum * RadiusSum, D) << std::endl;
		return D <= RadiusSum * RadiusSum;
	}

	bool CollisionChecker::TestSphereBoxCollision(const glm::vec3& InSphereCenter, const SphereShape* InSphere, const glm::vec3& InBoxCenter, const BoxShape* InBox)
	{
		Math::AABB BoundingBox = InBox->GetBoundingBox();
		BoundingBox.Offset(InBoxCenter);

		glm::vec3 ClosestPoint = glm::max(BoundingBox.MinBound, glm::min(InSphereCenter, BoundingBox.MaxBound));
		glm::vec3 Distance = ClosestPoint - InSphereCenter;

		return glm::dot(Distance, Distance) < InSphere->GetRadius() * InSphere->GetRadius();
	}

	bool CollisionChecker::TestBoxBoxCollision(const glm::vec3& InBoxCenter0, const BoxShape* InBox0, const glm::vec3& InBoxCenter1, const BoxShape* InBox1)
	{
		Math::AABB BoundingBox0 = InBox0->GetBoundingBox();
		Math::AABB BoundingBox1 = InBox1->GetBoundingBox();

		BoundingBox0.Offset(InBoxCenter0);
		BoundingBox1.Offset(InBoxCenter1);

		return BoundingBox0.IsAABBInside(BoundingBox1);
	}

}
