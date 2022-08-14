#include "FusionPCH.h"
#include "ByteBuffer.h"

namespace Fusion {

	ByteBuffer::ByteBuffer(uint32_t InSize)
		: Data(nullptr), Size(InSize)
	{
		Allocate(InSize);
	}

	ByteBuffer::ByteBuffer(const ByteBuffer& InOther)
		: Data(nullptr), Size(InOther.Size)
	{
		Data = new Byte[InOther.Size];
		memcpy(Data, InOther.Data, InOther.Size);
	}

	ByteBuffer::ByteBuffer(ByteBuffer&& InOther) noexcept
		: Data(InOther.Data), Size(InOther.Size)
	{
		InOther.Data = nullptr;
		InOther.Size = 0;
	}

	ByteBuffer& ByteBuffer::operator=(const ByteBuffer& InOther)
	{
		Release();
		Data = new Byte[InOther.Size];
		Size = InOther.Size;
		return *this;
	}
	
	ByteBuffer& ByteBuffer::operator=(ByteBuffer&& InOther) noexcept
	{
		Release();

		Data = InOther.Data;
		Size = InOther.Size;

		InOther.Data = nullptr;
		InOther.Size = 0;

		return *this;
	}

	ByteBuffer::~ByteBuffer()
	{
		Release();
	}

	void ByteBuffer::Release()
	{
		delete[] Data;
		Data = nullptr;
		Size = 0;
	}

	void ByteBuffer::Allocate(uint32_t InSize)
	{
		if (Data != nullptr)
			Release();

		FUSION_CORE_VERIFY(InSize > 0);
		Data = new Byte[InSize];
		Size = InSize;
		memset(Data, 0, InSize);
	}
}
