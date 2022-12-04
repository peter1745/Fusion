#include "FusionPCH.hpp"
#include "VulkanShader.hpp"
#include "VulkanContext.hpp"

#include <ranges>

namespace Fusion {

	VulkanShader::VulkanShader(const CompiledShaderData& InData)
	    : m_CompiledData(InData)
	{
	}

	void VulkanShader::Bind()
	{
	}

	void VulkanShader::Unbind()
	{
	}
}