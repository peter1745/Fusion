#include "FusionPCH.hpp"
#include "Shader.hpp"
#include "GraphicsContext.hpp"

#include <ranges>

namespace Fusion {

	Shader::Shader(const Shared<Device>& InDevice, const CompiledShaderData& InData)
	    : m_CompiledData(InData)
	{
		for (const auto& [ShaderStage, ByteCode] : InData.ModuleByteCodes)
		{
			VkShaderModuleCreateInfo ModuleCreateInfo = {};
			ModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			ModuleCreateInfo.codeSize = ByteCode.size() * sizeof(uint32_t);
			ModuleCreateInfo.pCode = ByteCode.data();
			vkCreateShaderModule(InDevice->GetLogicalDevice(), &ModuleCreateInfo, nullptr, &m_ShaderModules[ShaderStage]);
		}
	}

	void Shader::Bind()
	{
	}

	void Shader::Unbind()
	{
	}
}