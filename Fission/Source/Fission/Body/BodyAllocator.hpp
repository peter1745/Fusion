#pragma once

#include "Fission/Body/Body.hpp"

#include <vector>

namespace Fission {

	class BodyAllocator
	{
	public:
		BodyAllocator(uint32_t InMaxBodies);
		~BodyAllocator();

		Body* AllocateBody();
		void RemoveBody(const BodyID& InBodyID);

		Body* GetBody(const BodyID& InBodyID);

	private:
		uint32_t m_MaxBodies = 0;
		uint32_t m_NumBodies = 0;
		std::vector<Body> m_Bodies;

		uintptr_t m_FreeListStart = s_BodyFreeListEnd;

	private:
		static constexpr uintptr_t s_IsFreedBody = uintptr_t(1);
		static constexpr uint32_t s_FreedBodyMask = 1;
		static constexpr uintptr_t s_BodyFreeListEnd = ~uintptr_t(0);
	};

}
