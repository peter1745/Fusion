#include "BodyAllocator.hpp"

namespace Fission {

	BodyAllocator::BodyAllocator(uint32_t InMaxBodies)
		: m_MaxBodies(InMaxBodies)
	{
		m_Bodies.resize(InMaxBodies);
	}

	BodyAllocator::~BodyAllocator()
	{
		m_Bodies.clear();
	}

	Body* BodyAllocator::AllocateBody()
	{
		// Find next free index

		Body* NewBody = nullptr;

		uint32_t FreeIdx;
		if (m_FreeListStart != s_BodyFreeListEnd)
		{
			FreeIdx = uint32_t(m_FreeListStart >> s_FreedBodyMask);
			m_FreeListStart = *reinterpret_cast<const uintptr_t* const>(&m_Bodies[FreeIdx]);
			
			NewBody = &m_Bodies[FreeIdx];
			memset(NewBody, 0, sizeof(Body));
			
			NewBody->ID = BodyID(FreeIdx, 0);
		}
		else
		{
			if (m_NumBodies == m_MaxBodies)
				return nullptr;

			FreeIdx = m_NumBodies;

			NewBody = &m_Bodies[FreeIdx];
			memset(NewBody, 0, sizeof(Body));

			NewBody->ID = BodyID(FreeIdx, 0);
		}

		m_NumBodies++;
		return NewBody;
	}

	void BodyAllocator::RemoveBody(const BodyID& InBodyID)
	{
		uint32_t BodyIdx = InBodyID.GetIndex();
		Body* BodyPtr = &m_Bodies[BodyIdx];
		*reinterpret_cast<uintptr_t*>(BodyPtr) = m_FreeListStart;
		m_FreeListStart = (uintptr_t(BodyIdx) << s_FreedBodyMask) | s_IsFreedBody;
		m_NumBodies--;
	}

	Body* BodyAllocator::GetBody(const BodyID& InBodyID)
	{
		uint32_t BodyIdx = InBodyID.GetIndex();

		if (BodyIdx > m_Bodies.size())
			return nullptr;

		return &m_Bodies[BodyIdx];
	}

}
