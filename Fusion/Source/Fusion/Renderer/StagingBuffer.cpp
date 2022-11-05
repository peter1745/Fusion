#include "FusionPCH.hpp"
#include "StagingBuffer.hpp"
#include "Renderer.hpp"

#include <Platform/D3D12/D3D12StagingBuffer.hpp>

namespace Fusion {

	Unique<StagingBuffer> StagingBuffer::Create(const StagingBufferInfo& InCreateInfo)
	{
		switch (Renderer::CurrentAPI())
		{
		case ERendererAPI::None: return nullptr;
		case ERendererAPI::D3D11: return nullptr;
		case ERendererAPI::D3D12: return MakeUnique<D3D12StagingBuffer>(InCreateInfo);
		}

		return nullptr;
	}

}
