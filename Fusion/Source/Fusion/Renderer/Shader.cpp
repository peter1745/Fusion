#include "FusionPCH.hpp"
#include "Shader.hpp"

#include "RenderSettings.hpp"

#include "Platform/D3D11/D3D11Shader.hpp"
#include "Platform/D3D12/D3D12Shader.hpp"

namespace Fusion {

	Shared<Shader> Shader::Create(const ShaderSpecification& InSpecification)
	{
		switch (RenderSettings::Get().API)
		{
		case ERendererAPI::None: return nullptr;
		case ERendererAPI::D3D11: return Shared<D3D11Shader>::Create(InSpecification);
		case ERendererAPI::D3D12: return Shared<D3D12Shader>::Create(InSpecification);
		}

		return nullptr;
	}

}
