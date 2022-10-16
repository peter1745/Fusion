#include "FusionPCH.h"
#include "RenderTexture.h"
#include "Renderer.h"

#include "Platform/D3D11/D3D11RenderTexture.h"

namespace Fusion {

	Shared<RenderTexture> RenderTexture::Create(const RenderTextureInfo& InCreateInfo)
	{
		switch (Renderer::CurrentAPI())
		{
		case ERendererAPI::D3D11: return Shared<D3D11RenderTexture>::Create(InCreateInfo);
		}

		return nullptr;
	}

}
