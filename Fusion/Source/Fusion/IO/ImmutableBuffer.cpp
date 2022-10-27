#include "FusionPCH.hpp"
#include "ImmutableBuffer.hpp"

namespace Fusion {

	ImmutableBuffer::ImmutableBuffer(Byte* InData, uint32_t InSize)
		: m_Data(InData), m_Size(InSize)
	{
	}

	ImmutableBuffer::ImmutableBuffer(const ImmutableBuffer& InOther)
		: m_Data(nullptr), m_Size(InOther.m_Size)
	{
		m_Data = new Byte[InOther.m_Size];
		memcpy(m_Data, InOther.m_Data, InOther.m_Size);
	}

	ImmutableBuffer::ImmutableBuffer(ImmutableBuffer&& InOther) noexcept
		: m_Data(InOther.m_Data), m_Size(InOther.m_Size)
	{
		InOther.m_Data = nullptr;
		InOther.m_Size = 0;
	}

	ImmutableBuffer& ImmutableBuffer::operator=(const ImmutableBuffer& InOther)
	{
		Release();
		m_Data = new Byte[InOther.m_Size];
		m_Size = InOther.m_Size;
		return *this;
	}
	
	ImmutableBuffer& ImmutableBuffer::operator=(ImmutableBuffer&& InOther) noexcept
	{
		Release();

		m_Data = InOther.m_Data;
		m_Size = InOther.m_Size;

		InOther.m_Data = nullptr;
		InOther.m_Size = 0;

		return *this;
	}

	ImmutableBuffer::~ImmutableBuffer()
	{
		Release();
	}

	void ImmutableBuffer::Release()
	{
		delete[] m_Data;
		m_Data = nullptr;
		m_Size = 0;
	}

}
