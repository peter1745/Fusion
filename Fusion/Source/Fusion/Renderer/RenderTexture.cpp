#include "FusionPCH.hpp"
#include "RenderTexture.hpp"
#include "Renderer.hpp"

#include "Platform/D3D11/D3D11RenderTexture.hpp"
#include "Platform/D3D12/D3D12RenderTexture.hpp"

namespace Fusion {

	Shared<RenderTexture> RenderTexture::Create(const RenderTextureInfo& InCreateInfo)
	{
		switch (Renderer::CurrentAPI())
		{
		case ERendererAPI::None: return nullptr;
		case ERendererAPI::D3D11: return Shared<D3D11RenderTexture>::Create(InCreateInfo);
		case ERendererAPI::D3D12: return Shared<D3D12RenderTexture>::Create(InCreateInfo);
		}

		return nullptr;
	}

}
