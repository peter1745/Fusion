#include "FusionPCH.hpp"
#include "StagingBuffer.hpp"
#include "RenderSettings.hpp"

#include <Platform/D3D11/D3D11StagingBuffer.hpp>
#include <Platform/D3D12/D3D12StagingBuffer.hpp>

namespace Fusion {

	Unique<StagingBuffer> StagingBuffer::Create(const StagingBufferInfo& InCreateInfo)
	{
		switch (RenderSettings::Get().API)
		{
		case ERendererAPI::None: return nullptr;
		case ERendererAPI::D3D11: return MakeUnique<D3D11StagingBuffer>(InCreateInfo);
		case ERendererAPI::D3D12: return MakeUnique<D3D12StagingBuffer>(InCreateInfo);
		}

		return nullptr;
	}

}
