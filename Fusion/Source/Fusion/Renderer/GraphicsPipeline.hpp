#pragma once

#include "Common.hpp"
#include "GraphicsContext.hpp"
#include "Shader.hpp"

namespace Fusion {

	enum class EPrimitiveTopology
	{
		Triangles
	};

	enum class EWindingOrder
	{
		Clockwise,
		CounterClockwise
	};

	static constexpr uint32_t AppendAlignedElement = 0xFFFFFFFF;

	struct RenderTargetBlendStateInfo
	{
		bool EnableBlending = false;
	};

	struct GraphicsPipelineInfo
	{
		Shared<Shader> PipelineShader;

		EPrimitiveTopology PrimitiveTopology;
		EWindingOrder WindingOrder;

		uint8_t RenderTargetCount;
		std::array<EFormat, 8> RenderTargetFormats;
		std::array<RenderTargetBlendStateInfo, 8> RenderTargetBlendStates;
		EFormat DepthStencilFormat = EFormat::Unknown;
	};

	class GraphicsPipeline : public SharedObject
	{
	public:
		GraphicsPipeline(const Shared<GraphicsContext>& InContext, const GraphicsPipelineInfo& InCreateInfo);

		void Bind(CommandBuffer* InCmdList);

		const GraphicsPipelineInfo& GetInfo() const { return m_CreateInfo; }

		VkPipelineLayout GetPipelineLayout() const { return m_Layout; }

		VkDescriptorSet GetDescriptorSet(uint32_t InSetIndex, uint32_t InFrameIdx) const { return m_DescriptorSets[InFrameIdx][InSetIndex]; }

	private:
		GraphicsPipelineInfo m_CreateInfo;

		std::vector<VkDescriptorSetLayout> m_DescriptorSetLayouts;
		VkPipelineLayout m_Layout;
		VkPipeline m_Pipeline;
		VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
		std::vector<std::vector<VkDescriptorSet>> m_DescriptorSets;

	};

}
