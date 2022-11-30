#include "FusionPCH.hpp"
#include "GraphicsContext.hpp"
#include "RenderSettings.hpp"

#ifdef FUSION_PLATFORM_WINDOWS
	#include "Platform/D3D11/D3D11DescriptorHeap.hpp"
	#include "Platform/D3D12/D3D12DescriptorHeap.hpp"
#endif

namespace Fusion {

	Shared<GraphicsContext> GraphicsContext::Create()
	{
		switch (RenderSettings::Get().API)
		{
		case ERendererAPI::None: return nullptr;
#ifdef FUSION_PLATFORM_WINDOWS
		case ERendererAPI::D3D11: return Shared<D3D11Context>::Create();
		case ERendererAPI::D3D12: return Shared<D3D12Context>::Create();
#endif
		}

		return nullptr;
	}

}
