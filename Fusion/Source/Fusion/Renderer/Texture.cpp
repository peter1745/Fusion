#include "FusionPCH.hpp"
#include "Texture.hpp"
#include "RenderSettings.hpp"

#ifdef FUSION_PLATFORM_WINDOWS
	#include "Platform/D3D11/D3D11Texture.hpp"
	#include "Platform/D3D12/D3D12Texture.hpp"
#endif

namespace Fusion {

	Shared<Texture2D> Texture2D::Create(const Texture2DInfo& InCreateInfo)
	{
		switch (RenderSettings::Get().API)
		{
		case ERendererAPI::None: return nullptr;
#ifdef FUSION_PLATFORM_WINDOWS
		case ERendererAPI::D3D11: return Shared<D3D11Texture2D>::Create(InCreateInfo);
		case ERendererAPI::D3D12: return Shared<D3D12Texture2D>::Create(InCreateInfo);
#endif
		}

		return nullptr;
	}

}
