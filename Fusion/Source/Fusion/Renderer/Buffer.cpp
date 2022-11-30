#include "FusionPCH.hpp"
#include "Buffer.hpp"

#include "RenderSettings.hpp"

#ifdef FUSION_PLATFORM_WINDOWS
	#include "Platform/D3D11/D3D11Buffer.hpp"
	#include "Platform/D3D12/D3D12Buffer.hpp"
#endif

namespace Fusion {

	Shared<Buffer> Buffer::Create(const BufferInfo& InCreateInfo)
	{
		switch (RenderSettings::Get().API)
		{
		case ERendererAPI::None: return nullptr;
#ifdef FUSION_PLATFORM_WINDOWS
		case ERendererAPI::D3D11: return Shared<D3D11Buffer>::Create(InCreateInfo);
		case ERendererAPI::D3D12: return Shared<D3D12Buffer>::Create(InCreateInfo);
#endif
		}

		return nullptr;
	}

}
