#pragma once

#include <glm/glm.hpp>

#include <span>

namespace Fission {

	class AABB
	{
	public:
		static constexpr float InvalidMinBound = 1e6;
		static constexpr float InvalidMaxBound = -1e6;

	public:
		AABB() = default;
		AABB(const glm::vec3& InMinBound, const glm::vec3& InMaxBound);
		AABB(const AABB& InOther);
		AABB(AABB&& InOther) noexcept;

		AABB& operator=(const AABB& InOther);
		AABB& operator=(AABB&& InOther) noexcept;

		bool Intersects(const AABB& InOther) const;

		void Offset(const glm::vec3& InOffset);

		void Expand(std::span<AABB> InBounds);
		void Expand(const glm::vec3& InTarget);
		void Expand(const AABB& InOther);

		void Reset();

		const glm::vec3& GetMinBound() const { return m_MinBound; }
		const glm::vec3& GetMaxBound() const { return m_MaxBound; }

		float GetWidth() const { return m_MaxBound.x - m_MinBound.x; }
		float GetHeight() const { return m_MaxBound.y - m_MinBound.y; }
		float GetDepth() const { return m_MaxBound.z - m_MinBound.z; }

	private:
		glm::vec3 m_MinBound;
		glm::vec3 m_MaxBound;
	};

}
