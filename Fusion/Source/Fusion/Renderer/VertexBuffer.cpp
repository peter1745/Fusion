#include "FusionPCH.hpp"
#include "VertexBuffer.hpp"
#include "Renderer.hpp"

#include "Platform/D3D11/D3D11VertexBuffer.hpp"
#include "Platform/D3D12/D3D12VertexBuffer.hpp"

namespace Fusion {

	Shared<VertexBuffer> VertexBuffer::Create(const VertexBufferInfo& InCreateInfo)
	{
		switch (Renderer::CurrentAPI())
		{
		case ERendererAPI::None: return nullptr;
		case ERendererAPI::D3D11: return Shared<D3D11VertexBuffer>::Create(InCreateInfo);
		case ERendererAPI::D3D12: return Shared<D3D12VertexBuffer>::Create(InCreateInfo);
		}

		return nullptr;
	}

}
