#include "FusionPCH.hpp"
#include "ShaderCompiler.hpp"
#include "RenderSettings.hpp"

#include "Platform/Vulkan/VulkanShaderCompiler.hpp"

namespace Fusion {

	Unique<ShaderCompiler> ShaderCompiler::Create()
	{
		switch (RenderSettings::Get().API)
		{
		case ERendererAPI::None: return nullptr;
		case ERendererAPI::Vulkan: return MakeUnique<VulkanShaderCompiler>();
		}

		return nullptr;
	}

}
