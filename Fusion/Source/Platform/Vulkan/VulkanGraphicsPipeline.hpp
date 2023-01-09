#pragma once

#include "Fusion/Renderer/GraphicsPipeline.hpp"

#include "VulkanCommon.hpp"

namespace Fusion {

	class VulkanGraphicsPipeline : public GraphicsPipeline
	{
	public:
		VulkanGraphicsPipeline(const GraphicsPipelineInfo& InCreateInfo);

		void Bind(CommandList* InCmdList) override;

		const ResourceInfo& GetResourceInfo(const std::string& InName) const override { return m_Resources.at(InName); }
		const GraphicsPipelineInfo& GetInfo() const override { return m_CreateInfo; }

		VkPipelineLayout GetPipelineLayout() const { return m_Layout; }

	private:
		GraphicsPipelineInfo m_CreateInfo;

		VkPipelineLayout m_Layout;
		VkPipeline m_Pipeline;

		std::unordered_map<std::string, ResourceInfo> m_Resources;

	};

}
