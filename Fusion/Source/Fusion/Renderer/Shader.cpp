#include "FusionPCH.h"
#include "Shader.h"

#include "Platform/D3D11/D3D11Shader.h"

namespace Fusion {

	Shared<Shader> Shader::Create(const ShaderSpecification& InSpecification)
	{
		return Shared<D3D11Shader>::Create(InSpecification);
	}

}
