#pragma once

#include "Fusion/Core/Core.hpp"

namespace Fusion {

	struct ByteBuffer
	{
	public:
		ByteBuffer() = default;
		ByteBuffer(uint32_t InSize);
		ByteBuffer(const ByteBuffer& InOther);
		ByteBuffer(ByteBuffer&& InOther) noexcept;

		ByteBuffer& operator=(const ByteBuffer& InOther);
		ByteBuffer& operator=(ByteBuffer&& InOther) noexcept;

		~ByteBuffer();

		void Allocate(uint32_t InSize);
		void Release();

	public:
		Byte* Data = nullptr;
		uint32_t Size = 0;
	};

}
