#include "FusionPCH.hpp"
#include "ShaderCompiler.hpp"
#include "RenderSettings.hpp"

#ifdef FUSION_PLATFORM_WINDOWS
	#include "Platform/D3D11/D3D11ShaderCompiler.hpp"
	#include "Platform/D3D12/D3D12ShaderCompiler.hpp"
#endif

namespace Fusion {

	Unique<ShaderCompiler> ShaderCompiler::Create()
	{
		switch (RenderSettings::Get().API)
		{
		case ERendererAPI::None: return nullptr;
#ifdef FUSION_PLATFORM_WINDOWS
		case ERendererAPI::D3D11: return MakeUnique<D3D11ShaderCompiler>();
		case ERendererAPI::D3D12: return MakeUnique<D3D12ShaderCompiler>();
#endif
		case ERendererAPI::Vulkan: return nullptr;
		}

		return nullptr;
	}

}
