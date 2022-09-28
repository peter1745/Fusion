#include "FusionPCH.h"
#include "OpenGLIndexBuffer.h"

#include <glad/gl.h>

namespace Fusion {

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t InSize, void* InData)
		: m_Size(InSize), m_BufferID(0)
	{
		glCreateBuffers(1, &m_BufferID);
		glNamedBufferData(m_BufferID, InSize, InData, GL_STATIC_DRAW);
	}

}

