#pragma once

#include "BroadPhase.hpp"

namespace Fission {

	struct PsuedoBody
	{
		int32_t ID;
		float Value;
		bool IsMin;
	};

	class SweepAndPruneBroadPhase : public BroadPhase
	{
	public:
		virtual void Initialize(BodyAllocator* InBodyManager) override;
		virtual void AddBodies(const BodyID* InBodyIDs, uint32_t InBodyCount) override;

		virtual void Execute(float InDeltaTime) override;

		virtual const std::vector<BroadPhasePairs>& GetCollisionPairs() const override { return m_CollisionPairs; }

	private:
		BodyAllocator* m_BodyManager = nullptr;
		std::vector<BodyID> m_BodyIDs;

		std::vector<PsuedoBody> m_SortedBodies;
		std::vector<BroadPhasePairs> m_CollisionPairs;

	};

}

