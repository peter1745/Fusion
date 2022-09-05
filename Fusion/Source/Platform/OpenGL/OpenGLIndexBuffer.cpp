#include "FusionPCH.h"
#include "OpenGLIndexBuffer.h"

#include <glad/gl.h>

namespace Fusion {

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t InCount, uint32_t* InData)
		: m_Count(InCount), m_BufferID(0)
	{
		glCreateBuffers(1, &m_BufferID);
		glNamedBufferData(m_BufferID, InCount * sizeof(uint32_t), InData, GL_STATIC_DRAW);
	}

	void OpenGLIndexBuffer::Bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
	}

	void OpenGLIndexBuffer::Unbind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}

