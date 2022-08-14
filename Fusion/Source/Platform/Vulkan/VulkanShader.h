#pragma once

#include "Fusion/IO/ByteBuffer.h"
#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Fusion {

	struct VulkanShaderSpecification
	{
		std::filesystem::path VertexFilePath;
		std::filesystem::path FragmentFilePath;
	};

	class VulkanShader
	{
	public:
		static constexpr uint32_t MaxShaderTypes = 2;
		enum EShaderType { EShaderType_Vertex, EShaderType_Fragment };

	public:
		VulkanShader(const VulkanShaderSpecification& InSpecification, const Shared<VulkanDevice>& InDevice);
		~VulkanShader();

		const std::array<VkPipelineShaderStageCreateInfo, MaxShaderTypes>& GetShaderStages() const { return m_ShaderStages; }

	private:
		VkShaderModule CreateModule(const ByteBuffer& InData) const;

	private:
		VulkanShaderSpecification m_Specification;
		Shared<VulkanDevice> m_Device;
		VkShaderModule m_VertexModule;
		VkShaderModule m_FragmentModule;

		std::array<VkPipelineShaderStageCreateInfo, MaxShaderTypes> m_ShaderStages;
	};

}
