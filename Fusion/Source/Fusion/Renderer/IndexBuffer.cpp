#include "FusionPCH.h"
#include "IndexBuffer.h"
#include "Renderer.h"

#include "Platform/D3D11/D3D11IndexBuffer.h"

namespace Fusion {

	Shared<IndexBuffer> IndexBuffer::Create(uint32_t InSize, void* InData)
	{
		switch (Renderer::CurrentAPI())
		{
		case ERendererAPI::D3D11: return Shared<D3D11IndexBuffer>::Create(InData, InSize);
		}

		return nullptr;
	}

}
