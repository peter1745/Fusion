#pragma once

#include "Fusion/Core/Core.hpp"

namespace Fusion {

	struct ImmutableBuffer
	{
	public:
		ImmutableBuffer() = default;
		ImmutableBuffer(Byte* InData, uint32_t InSize);
		ImmutableBuffer(const ImmutableBuffer& InOther);
		ImmutableBuffer(ImmutableBuffer&& InOther) noexcept;

		ImmutableBuffer& operator=(const ImmutableBuffer& InOther);
		ImmutableBuffer& operator=(ImmutableBuffer&& InOther) noexcept;

		virtual ~ImmutableBuffer();

		void Release();

		template<typename T>
		T Read()
		{
			T Result;
			memcpy(&Result, m_Data + m_CurrentOffset, sizeof(T));
			m_CurrentOffset += sizeof(T);
			return Result;
		}

		const Byte* GetData() const { return m_Data; }
		uint32_t GetSize() const { return m_Size; }

	protected:
		Byte* m_Data = nullptr;
		uint32_t m_Size = 0;
		uint32_t m_CurrentOffset = 0;
	};

	struct WritableBuffer : public ImmutableBuffer
	{
		WritableBuffer(Byte* InData, uint32_t InSize)
			: ImmutableBuffer(InData, InSize) {}

		WritableBuffer(uint32_t InSize)
			: ImmutableBuffer()
		{
			m_Size = InSize;
			m_Data = new Byte[InSize];
		}

		template<typename T>
		void Write(const T& InValue)
		{
			CoreVerify(m_Data && m_Offset + sizeof(T) <= m_Size);
			memcpy(m_Data + m_Offset, &InValue, sizeof(T));
			m_Offset += sizeof(T);
		}

	private:
		uint32_t m_Offset = 0;
	};
		
}
