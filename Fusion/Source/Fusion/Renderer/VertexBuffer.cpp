#include "FusionPCH.h"
#include "VertexBuffer.h"
#include "Renderer.h"

#include "Platform/D3D11/D3D11VertexBuffer.h"

namespace Fusion {

	Shared<VertexBuffer> VertexBuffer::Create(const VertexBufferInfo& InCreateInfo)
	{
		switch (Renderer::CurrentAPI())
		{
		case ERendererAPI::D3D11: return Shared<D3D11VertexBuffer>::Create(InCreateInfo);
		}

		return nullptr;
	}

}
