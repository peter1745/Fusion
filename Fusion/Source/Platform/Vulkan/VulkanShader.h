#pragma once

#include "Fusion/Renderer/Shader.h"
#include "Fusion/IO/ByteBuffer.h"
#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Fusion {

	class VulkanShader : public Shader
	{
	public:
		static constexpr uint32_t VertexShaderID = 0;
		static constexpr uint32_t FragmentShaderID = 1;
		static constexpr uint32_t MaxShaderTypes = 2;

	public:
		VulkanShader(const ShaderSpecification& InSpecification, const Shared<VulkanDevice>& InDevice);
		~VulkanShader();

		virtual const ShaderSpecification& GetSpecification() const override { return m_Specification; }

		const std::array<VkPipelineShaderStageCreateInfo, MaxShaderTypes>& GetShaderStages() const { return m_ShaderStages; }

	private:
		ShaderSpecification m_Specification;
		Shared<VulkanDevice> m_Device;
		VkShaderModule m_VertexModule = VK_NULL_HANDLE;
		VkShaderModule m_FragmentModule = VK_NULL_HANDLE;

		std::array<VkPipelineShaderStageCreateInfo, MaxShaderTypes> m_ShaderStages;
	};

}
