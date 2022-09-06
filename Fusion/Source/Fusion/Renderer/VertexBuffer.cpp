#include "FusionPCH.h"
#include "VertexBuffer.h"

#include "Platform/OpenGL/OpenGLVertexBuffer.h"

namespace Fusion {

	Shared<VertexBuffer> VertexBuffer::Create(uint32_t InSize, void* InData, const VertexBufferLayout& InLayout)
	{
		return Shared<OpenGLVertexBuffer>::Create(InSize, InData, InLayout);
	}

}
