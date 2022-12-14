#include "FusionPCH.hpp"
#include "GraphicsContext.hpp"
#include "RenderSettings.hpp"

#include "Platform/D3D11/D3D11Context.hpp"
#include "Platform/D3D12/D3D12Context.hpp"

namespace Fusion {

	Shared<GraphicsContext> GraphicsContext::Create()
	{
		switch (RenderSettings::Get().API)
		{
		case ERendererAPI::None: return nullptr;
		case ERendererAPI::D3D11: return Shared<D3D11Context>::Create();
		case ERendererAPI::D3D12: return Shared<D3D12Context>::Create();
		}

		return nullptr;
	}

}
