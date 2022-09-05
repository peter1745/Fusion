#pragma once

#include "Fusion/Renderer/VertexBuffer.h"

namespace Fusion {

	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t InSize, void* InData, const VertexBufferLayout& InLayout);
		~OpenGLVertexBuffer();

		virtual void Bind() override;
		virtual void Unbind() override;

	private:
		uint32_t m_VertexArrayID = 0;
		uint32_t m_BufferID = 0;

		VertexBufferLayout m_Layout;
	};

}
