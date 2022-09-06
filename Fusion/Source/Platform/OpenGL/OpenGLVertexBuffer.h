#pragma once

#include "Fusion/Renderer/VertexBuffer.h"

namespace Fusion {

	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t InSize, void* InData, const VertexBufferLayout& InLayout);
		~OpenGLVertexBuffer();

		uint32_t GetVertexArrayID() const { return m_VertexArrayID; }
		uint32_t GetBufferID() const { return m_BufferID; }
		virtual const VertexBufferLayout& GetLayout() const override { return m_Layout; }

	private:
		uint32_t m_VertexArrayID = 0;
		uint32_t m_BufferID = 0;

		VertexBufferLayout m_Layout;
	};

}
