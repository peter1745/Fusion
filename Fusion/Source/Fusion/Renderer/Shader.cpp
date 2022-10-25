#include "FusionPCH.hpp"
#include "Shader.hpp"

#include "Platform/D3D11/D3D11Shader.hpp"

namespace Fusion {

	Shared<Shader> Shader::Create(const ShaderSpecification& InSpecification)
	{
		return Shared<D3D11Shader>::Create(InSpecification);
	}

}
