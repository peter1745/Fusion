#include "FusionPCH.h"
#include "IndexBuffer.h"

#include "Platform/OpenGL/OpenGLIndexBuffer.h"

namespace Fusion {

	Shared<IndexBuffer> IndexBuffer::Create(uint32_t InCount, uint32_t* InData)
	{
		return Shared<OpenGLIndexBuffer>::Create(InCount, InData);
	}

}
