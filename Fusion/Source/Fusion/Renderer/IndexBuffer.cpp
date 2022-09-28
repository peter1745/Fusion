#include "FusionPCH.h"
#include "IndexBuffer.h"

#include "Platform/OpenGL/OpenGLIndexBuffer.h"

namespace Fusion {

	Shared<IndexBuffer> IndexBuffer::Create(uint32_t InSize, void* InData)
	{
		return Shared<OpenGLIndexBuffer>::Create(InSize, InData);
	}

}
