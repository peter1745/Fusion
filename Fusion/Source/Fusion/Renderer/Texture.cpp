#include "FusionPCH.hpp"
#include "Texture.hpp"
#include "Renderer.hpp"

#ifdef FUSION_PLATFORM_WINDOWS
	#include "Platform/D3D11/D3D11Texture.hpp"
#endif

namespace Fusion {

	Shared<Texture2D> Texture2D::Create(const Texture2DInfo& InCreateInfo)
	{
		switch (Renderer::CurrentAPI())
		{
		case ERendererAPI::D3D11: return Shared<D3D11Texture2D>::Create(InCreateInfo);
		}

		return nullptr;
	}

}
