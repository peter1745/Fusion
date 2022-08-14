#include "FusionPCH.h"
#include "VulkanShader.h"

#include "Fusion/IO/FileIO.h"

namespace Fusion {

	VulkanShader::VulkanShader(const VulkanShaderSpecification& InSpecification, const Shared<VulkanDevice>& InDevice)
		: m_Specification(InSpecification), m_Device(InDevice)
	{
		ByteBuffer VertexDataBuffer;
		ByteBuffer FragmentDataBuffer;
		FUSION_CORE_VERIFY(FileIO::ReadFile(m_Specification.VertexFilePath, VertexDataBuffer));
		FUSION_CORE_VERIFY(FileIO::ReadFile(m_Specification.FragmentFilePath, FragmentDataBuffer));

		m_VertexModule = CreateModule(VertexDataBuffer);
		m_FragmentModule = CreateModule(FragmentDataBuffer);

		m_ShaderStages[EShaderType_Vertex] = {};
		m_ShaderStages[EShaderType_Vertex].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		m_ShaderStages[EShaderType_Vertex].stage = VK_SHADER_STAGE_VERTEX_BIT;
		m_ShaderStages[EShaderType_Vertex].module = m_VertexModule;
		m_ShaderStages[EShaderType_Vertex].pName = "main";

		m_ShaderStages[EShaderType_Fragment] = {};
		m_ShaderStages[EShaderType_Fragment].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		m_ShaderStages[EShaderType_Fragment].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		m_ShaderStages[EShaderType_Fragment].module = m_FragmentModule;
		m_ShaderStages[EShaderType_Fragment].pName = "main";
	}

	VulkanShader::~VulkanShader()
	{
		vkDestroyShaderModule(m_Device->GetLogicalDevice(), m_FragmentModule, nullptr);
		vkDestroyShaderModule(m_Device->GetLogicalDevice(), m_VertexModule, nullptr);
	}

	VkShaderModule VulkanShader::CreateModule(const ByteBuffer& InData) const
	{
		VkShaderModuleCreateInfo ModuleCreateInfo = {};
		ModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		ModuleCreateInfo.codeSize = InData.Size;
		ModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(InData.Data);

		VkShaderModule ShaderModule = VK_NULL_HANDLE;
		FUSION_CORE_VERIFY(vkCreateShaderModule(m_Device->GetLogicalDevice(), &ModuleCreateInfo, nullptr, &ShaderModule) == VK_SUCCESS);
		return ShaderModule;
	}

}
