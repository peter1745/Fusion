#pragma once

#include "Fusion/Renderer/IndexBuffer.h"

namespace Fusion {

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t InCount, uint32_t* InData);

		virtual uint32_t GetCount() const override { return m_Count; }
		uint32_t GetBufferID() const { return m_BufferID; }

	private:
		uint32_t m_Count = 0;
		uint32_t m_BufferID = 0;
	};

}
