#include "FusionPCH.hpp"
#include "IndexBuffer.hpp"
#include "Renderer.hpp"

#include "Platform/D3D11/D3D11IndexBuffer.hpp"

namespace Fusion {

	Shared<IndexBuffer> IndexBuffer::Create(uint32_t InSize, void* InData)
	{
		switch (Renderer::CurrentAPI())
		{
		case ERendererAPI::None: return nullptr;
		case ERendererAPI::D3D11: return Shared<D3D11IndexBuffer>::Create(InData, InSize);
		}

		return nullptr;
	}

}
