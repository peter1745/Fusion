#include "SweepAndPruneBroadPhase.hpp"

#include <cpp-sort/sorters/quick_sorter.h>
#include <tracy/Tracy.hpp>

namespace Fission {

	void SweepAndPruneBroadPhase::Initialize(BodyAllocator* InBodyManager)
	{
		m_BodyManager = InBodyManager;
	}

	void SweepAndPruneBroadPhase::AddBodies(const BodyID* InBodyIDs, uint32_t InBodyCount)
	{
		for (size_t Idx = 0; Idx < InBodyCount; Idx++)
			m_BodyIDs.push_back(InBodyIDs[Idx]);

		m_SortedBodies.resize(m_BodyIDs.size() * 2);
	}

	void SweepAndPruneBroadPhase::Execute(float InDeltaTime)
	{
		ZoneScoped;

		m_CollisionPairs.clear();

		// Sort the body bounds
		{
			ZoneScopedN("Bounds Expansion");

			const glm::vec3 Axis = { 1.0f, 1.0f, 1.0f };

			for (size_t Idx = 0; Idx < m_BodyIDs.size(); Idx++)
			{
				const Body* ActiveBody = m_BodyManager->GetBody(m_BodyIDs[Idx]);
				AABB BoundingVolume = ActiveBody->Shape->GetBoundingBox(ActiveBody->Location, ActiveBody->Orientation);

				BoundingVolume.Expand(BoundingVolume.GetMinBound() + ActiveBody->LinearVelocity * InDeltaTime);
				BoundingVolume.Expand(BoundingVolume.GetMaxBound() + ActiveBody->LinearVelocity * InDeltaTime);

				const float Epsilon = 0.01f;
				BoundingVolume.Expand(BoundingVolume.GetMinBound() + glm::vec3(-1.0f) * Epsilon);
				BoundingVolume.Expand(BoundingVolume.GetMaxBound() + glm::vec3(1.0f) * Epsilon);

				m_SortedBodies[Idx * 2 + 0].ID = Idx;
				m_SortedBodies[Idx * 2 + 0].Value = glm::dot(Axis, BoundingVolume.GetMinBound());
				m_SortedBodies[Idx * 2 + 0].IsMin = true;

				m_SortedBodies[Idx * 2 + 1].ID = Idx;
				m_SortedBodies[Idx * 2 + 1].Value = glm::dot(Axis, BoundingVolume.GetMaxBound());
				m_SortedBodies[Idx * 2 + 1].IsMin = false;
			}

			{
				ZoneScopedN("Bounds Sorting");
				cppsort::quick_sort(m_SortedBodies, [](const auto& Body0, const auto& Body1)
				{
					return Body0.Value < Body1.Value;
				});
			}
		}

		// Build the collision pairs
		{
			ZoneScopedN("Build Collision Pairs");

			for (size_t Idx = 0; Idx < m_SortedBodies.size(); Idx++)
			{
				const PsuedoBody& Body0 = m_SortedBodies[Idx];

				if (!Body0.IsMin)
					continue;

				BroadPhasePairs Pair = {};
				Pair.IDIndex0 = Body0.ID;

				for (uint32_t J = Idx + 1; J < m_SortedBodies.size(); J++)
				{
					const PsuedoBody& Body1 = m_SortedBodies[J];

					if (Body1.ID == Body0.ID)
						break;

					if (!Body1.IsMin)
						continue;

					Pair.IDIndex1 = Body1.ID;
					m_CollisionPairs.push_back(Pair);
				}
			}
		}
	}

}
