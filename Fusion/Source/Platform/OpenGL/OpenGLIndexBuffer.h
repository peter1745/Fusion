#pragma once

#include "Fusion/Renderer/IndexBuffer.h"

namespace Fusion {

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t InSize, void* InData);

		virtual uint32_t GetCount() const override { return m_Size / sizeof(uint32_t); }
		uint32_t GetBufferID() const { return m_BufferID; }

	private:
		uint32_t m_Size = 0;
		uint32_t m_BufferID = 0;
	};

}
