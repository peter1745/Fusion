#include "FusionPCH.hpp"
#include "D3D12Shader.hpp"
#include "D3D12Context.hpp"

#include <d3dcompiler.h>

namespace Fusion {

	D3D12Shader::D3D12Shader(const CompiledShaderData& InCompiledData)
		: m_CompiledShaderData(InCompiledData)
	{
	}

	D3D12Shader::~D3D12Shader()
	{
	}

	
}
