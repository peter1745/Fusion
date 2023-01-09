#include "FusionPCH.hpp"
#include "VulkanShader.hpp"
#include "VulkanContext.hpp"

#include <ranges>

namespace Fusion {

	VulkanShader::VulkanShader(const VulkanCompiledShaderData& InData)
	    : m_CompiledData(InData)
	{
		auto Device = GraphicsContext::Get<VulkanContext>()->GetDevice().As<VulkanDevice>();

		for (const auto& [ShaderStage, ByteCode] : InData.ModuleByteCodes)
		{
			VkShaderModuleCreateInfo ModuleCreateInfo = {};
			ModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			ModuleCreateInfo.codeSize = ByteCode.size() * sizeof(uint32_t);
			ModuleCreateInfo.pCode = ByteCode.data();
			vkCreateShaderModule(Device->GetLogicalDevice(), &ModuleCreateInfo, nullptr, &m_ShaderModules[ShaderStage]);
		}
	}

	void VulkanShader::Bind()
	{
	}

	void VulkanShader::Unbind()
	{
	}
}