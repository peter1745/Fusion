#include "FusionPCH.h"
#include "VulkanShader.h"

#include "Fusion/IO/FileIO.h"

namespace Fusion {

	VulkanShader::VulkanShader(const ShaderSpecification& InSpecification, const Shared<VulkanDevice>& InDevice)
		: m_Specification(InSpecification), m_Device(InDevice)
	{
		// Create Shader Modules
		{
			ByteBuffer VertexDataBuffer;
			FUSION_CORE_VERIFY(FileIO::ReadFile(m_Specification.VertexFilePath, VertexDataBuffer));
			VkShaderModuleCreateInfo VertexModuleCreateInfo = {};
			VertexModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			VertexModuleCreateInfo.codeSize = VertexDataBuffer.Size;
			VertexModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(VertexDataBuffer.Data);
			FUSION_CORE_VERIFY(vkCreateShaderModule(m_Device->GetLogicalDevice(), &VertexModuleCreateInfo, nullptr, &m_VertexModule) == VK_SUCCESS);

			ByteBuffer FragmentDataBuffer;
			FUSION_CORE_VERIFY(FileIO::ReadFile(m_Specification.FragmentFilePath, FragmentDataBuffer));
			VkShaderModuleCreateInfo FragmentModuleCreateInfo = {};
			FragmentModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			FragmentModuleCreateInfo.codeSize = FragmentDataBuffer.Size;
			FragmentModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(FragmentDataBuffer.Data);
			FUSION_CORE_VERIFY(vkCreateShaderModule(m_Device->GetLogicalDevice(), &FragmentModuleCreateInfo, nullptr, &m_FragmentModule) == VK_SUCCESS);
		}

		// Shader Stages
		{
			m_ShaderStages[VertexShaderID] = {};
			m_ShaderStages[VertexShaderID].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			m_ShaderStages[VertexShaderID].stage = VK_SHADER_STAGE_VERTEX_BIT;
			m_ShaderStages[VertexShaderID].module = m_VertexModule;
			m_ShaderStages[VertexShaderID].pName = "main";

			m_ShaderStages[FragmentShaderID] = {};
			m_ShaderStages[FragmentShaderID].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			m_ShaderStages[FragmentShaderID].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			m_ShaderStages[FragmentShaderID].module = m_FragmentModule;
			m_ShaderStages[FragmentShaderID].pName = "main";
		}
	}

	VulkanShader::~VulkanShader()
	{
		vkDestroyShaderModule(m_Device->GetLogicalDevice(), m_FragmentModule, nullptr);
		vkDestroyShaderModule(m_Device->GetLogicalDevice(), m_VertexModule, nullptr);
	}

}
