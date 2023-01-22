#include "FissionPCH.hpp"
#include "AABB.hpp"

namespace Fission::Math {

	bool AABB::IsPointInside(const glm::vec3& InPoint) const
	{
		if (InPoint.x < MinBound.x || InPoint.y < MinBound.y || InPoint.z < MinBound.z)
		{
			return false;
		}

		if (InPoint.x > MaxBound.x || InPoint.y > MaxBound.y || InPoint.z > MaxBound.z)
		{
			return false;
		}

		return true;
	}

	bool AABB::IsAABBInside(const AABB& InOther) const
	{
		if (MinBound.x > InOther.MaxBound.x || MinBound.y > InOther.MaxBound.y || MinBound.z > InOther.MaxBound.z)
		{
			return false;
		}

		if (MaxBound.x < InOther.MinBound.x || MaxBound.y < InOther.MinBound.y || MaxBound.z < InOther.MinBound.z)
		{
			return false;
		}

		return true;
	}

	void AABB::Offset(const glm::vec3& InOffset)
	{
		MinBound += InOffset;
		MaxBound += InOffset;
	}

}
