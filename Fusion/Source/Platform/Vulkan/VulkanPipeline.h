#pragma once

#include "VulkanDevice.h"
#include "VulkanShader.h"
#include <vulkan/vulkan.h>

namespace Fusion {

	struct PipelineSpecification
	{
		VkRenderPass RenderPass;
		Shared<VulkanShader> PipelineShader;
	};

	class VulkanPipeline
	{
	public:
		VulkanPipeline(const PipelineSpecification& InSpecification, const Shared<VulkanDevice>& InDevice);
		~VulkanPipeline();

		VkPipeline GetPipeline() const { return m_Pipeline; }

	private:
		PipelineSpecification m_Specification;
		Shared<VulkanDevice> m_Device = nullptr;
		VkPipelineLayout m_Layout = VK_NULL_HANDLE;
		VkPipeline m_Pipeline = VK_NULL_HANDLE;
	};

}
