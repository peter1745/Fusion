#include "AABB.hpp"

namespace Fission {

	AABB::AABB(const glm::vec3& InMinBound, const glm::vec3& InMaxBound)
		: m_MinBound(InMinBound), m_MaxBound(InMaxBound)
	{
	}

	AABB::AABB(const AABB& InOther)
		: m_MinBound(InOther.m_MinBound), m_MaxBound(InOther.m_MaxBound)
	{
	}

	AABB::AABB(AABB&& InOther) noexcept
	{
		m_MinBound = std::move(InOther.m_MinBound);
		m_MaxBound = std::move(InOther.m_MaxBound);
	}

	bool AABB::Intersects(const AABB& InOther) const
	{
		if (m_MaxBound.x < InOther.m_MinBound.x || m_MaxBound.y < InOther.m_MinBound.y || m_MaxBound.z < InOther.m_MinBound.z)
			return false;

		if (InOther.m_MaxBound.x < m_MinBound.x || InOther.m_MaxBound.y < m_MinBound.y || InOther.m_MaxBound.z < m_MinBound.z)
			return false;

		return true;
	}

	void AABB::Offset(const glm::vec3& InOffset)
	{
		m_MinBound += InOffset;
		m_MaxBound += InOffset;
	}

	void AABB::Expand(std::span<AABB> InBounds)
	{
		for (const auto& OtherBound : InBounds)
			Expand(OtherBound);
	}

	void AABB::Expand(const glm::vec3& InTarget)
	{
		if (InTarget.x < m_MinBound.x)
			m_MinBound.x = InTarget.x;

		if (InTarget.y < m_MinBound.y)
			m_MinBound.y = InTarget.y;

		if (InTarget.z < m_MinBound.z)
			m_MinBound.z = InTarget.z;

		if (InTarget.x > m_MaxBound.x)
			m_MaxBound.x = InTarget.x;

		if (InTarget.y > m_MaxBound.y)
			m_MaxBound.y = InTarget.y;

		if (InTarget.z > m_MaxBound.z)
			m_MaxBound.z = InTarget.z;
	}

	void AABB::Expand(const AABB& InOther)
	{
		Expand(InOther.m_MinBound);
		Expand(InOther.m_MaxBound);
	}

	/*void AABB::Expand(std::span<glm::vec3> InPoints)
	{
		for (const auto& Point : InPoints)
			Expand(Point);
	}*/

	void AABB::Expand(std::span<const glm::vec3> InPoints)
	{
		for (const auto& Point : InPoints)
			Expand(Point);
	}

	void AABB::Reset()
	{
		m_MinBound = glm::vec3(InvalidMinBound);
		m_MaxBound = glm::vec3(InvalidMaxBound);
	}

	AABB& AABB::operator=(AABB&& InOther) noexcept
	{
		m_MinBound = std::move(InOther.m_MinBound);
		m_MaxBound = std::move(InOther.m_MaxBound);
		return *this;
	}

	AABB& AABB::operator=(const AABB& InOther)
	{
		m_MinBound = InOther.m_MinBound;
		m_MaxBound = InOther.m_MaxBound;
		return *this;
	}

}
