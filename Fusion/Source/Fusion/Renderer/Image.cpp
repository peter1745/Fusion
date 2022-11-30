#include "FusionPCH.hpp"
#include "Image.hpp"
#include "RenderSettings.hpp"

#ifdef FUSION_PLATFORM_WINDOWS
	#include "Platform/D3D12/D3D12Image2D.hpp"
#endif

namespace Fusion {

	Shared<Image2D> Image2D::Create(const Image2DInfo& InCreateInfo)
	{
		switch (RenderSettings::Get().API)
		{
		case ERendererAPI::None: return nullptr;
#ifdef FUSION_PLATFORM_WINDOWS
		case ERendererAPI::D3D11: return nullptr;
		case ERendererAPI::D3D12: return Shared<D3D12Image2D>::Create(InCreateInfo);
#endif
		}

		return nullptr;
	}

}
