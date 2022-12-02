#pragma once

#include "Fission/Body/BodyAllocator.hpp"

namespace Fission {

	enum class EBroadPhaseAlgorithm { SAP };

	struct BroadPhasePairs
	{
		int32_t IDIndex0;
		int32_t IDIndex1;

		bool operator==(const BroadPhasePairs& InOther)
		{
			return (IDIndex0 == InOther.IDIndex0 && IDIndex1 == InOther.IDIndex1) || (IDIndex0 == InOther.IDIndex1 && IDIndex1 == InOther.IDIndex0);
		}

		bool operator!=(const BroadPhasePairs& InOther)
		{
			return *this != InOther;
		}
	};

	class BroadPhase
	{
	public:
		virtual void Initialize(BodyAllocator* InBodyManager) = 0;
		virtual void AddBodies(const BodyID* InBodyIDs, uint32_t InBodyCount) = 0;

		virtual void Execute(float InDeltaTime) = 0;

		virtual const std::vector<BroadPhasePairs>& GetCollisionPairs() const = 0;

	};

}
