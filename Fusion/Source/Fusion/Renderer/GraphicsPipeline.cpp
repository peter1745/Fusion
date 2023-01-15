#include "FusionPCH.hpp"
#include "GraphicsPipeline.hpp"
#include "Shader.hpp"

namespace Fusion {

	static VkShaderStageFlags ShaderTypeToShaderStageFlags(EShaderType InType)
	{
		switch (InType)
		{
		case EShaderType::Vertex: return VK_SHADER_STAGE_VERTEX_BIT;
		case EShaderType::Pixel: return VK_SHADER_STAGE_FRAGMENT_BIT;
		}

		CoreVerify(false);
		return static_cast<VkShaderStageFlags>(-1);
	}

	static VkShaderStageFlagBits ShaderTypeToShaderStageFlagBits(EShaderType InType)
	{
		switch (InType)
		{
		case EShaderType::Vertex: return VK_SHADER_STAGE_VERTEX_BIT;
		case EShaderType::Pixel: return VK_SHADER_STAGE_FRAGMENT_BIT;
		}

		CoreVerify(false);
		return static_cast<VkShaderStageFlagBits>(-1);
	}

	static VkPrimitiveTopology PrimitiveTopologyToVkPrimitiveTopology(EPrimitiveTopology InTopology)
	{
		switch (InTopology)
		{
		case EPrimitiveTopology::Triangles: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		}

		CoreVerify(false);
		return static_cast<VkPrimitiveTopology>(-1);
	}

	GraphicsPipeline::GraphicsPipeline(const Shared<GraphicsContext>& InContext, const GraphicsPipelineInfo& InCreateInfo)
	    : m_CreateInfo(InCreateInfo)
	{
		std::vector<VkPipelineShaderStageCreateInfo> ShaderStageCreateInfos;
		std::vector<VkPushConstantRange> PushConstantRanges;

		const auto& ShaderModules = InCreateInfo.PipelineShader->GetShaderModules();

		std::vector<VkVertexInputBindingDescription> InputBindings;
		std::vector<VkVertexInputAttributeDescription> InputAttributes;

		for (const auto& [ShaderStage, ReflectionData] : InCreateInfo.PipelineShader->GetReflectedModules())
		{
			auto& ShaderStageInfo = ShaderStageCreateInfos.emplace_back();
			ShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			ShaderStageInfo.flags = 0;
			ShaderStageInfo.stage = ShaderTypeToShaderStageFlagBits(ShaderStage);
			ShaderStageInfo.module = ShaderModules.at(ShaderStage);
			ShaderStageInfo.pName = "main";

			if (ShaderStage == EShaderType::Vertex)
			{
				auto& InputBinding = InputBindings.emplace_back();
				InputBinding.binding = 0;

				uint32_t InputAttribOffset = 0;

				for (const auto& InputParam : ReflectionData.InputParameters)
				{
					auto& InputAttrib = InputAttributes.emplace_back();
					InputAttrib.location = InputParam.Location;
					InputAttrib.binding = 0;
					InputAttrib.format = EFormatToVkFormat(InputParam.Format);
					InputAttrib.offset = InputAttribOffset;

					InputAttribOffset += GetFormatSize(InputParam.Format);
				}

				InputBinding.stride = InputAttribOffset;
				InputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			}

			for (const auto& PushConstantInfo : ReflectionData.PushConstants)
			{
				auto& PushConstantRangeInfo = PushConstantRanges.emplace_back();
				PushConstantRangeInfo.stageFlags = ShaderTypeToShaderStageFlags(ShaderStage);
				PushConstantRangeInfo.offset = 0;
				PushConstantRangeInfo.size = PushConstantInfo.Size;
			}
		}

		VkPipelineVertexInputStateCreateInfo VertexInputInfo = {};
		VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		VertexInputInfo.vertexBindingDescriptionCount = InputBindings.size();
		VertexInputInfo.pVertexBindingDescriptions = InputBindings.data();
		VertexInputInfo.vertexAttributeDescriptionCount = InputAttributes.size();
		VertexInputInfo.pVertexAttributeDescriptions = InputAttributes.data();

		VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo = {};
		PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		PipelineLayoutCreateInfo.flags = 0;
		PipelineLayoutCreateInfo.setLayoutCount = 0;
		PipelineLayoutCreateInfo.pSetLayouts = nullptr;
		PipelineLayoutCreateInfo.pushConstantRangeCount = PushConstantRanges.size();
		PipelineLayoutCreateInfo.pPushConstantRanges = PushConstantRanges.data();

		vkCreatePipelineLayout(InContext->GetDevice()->GetLogicalDevice(), &PipelineLayoutCreateInfo, nullptr, &m_Layout);

		VkPipelineInputAssemblyStateCreateInfo InputAssemblyStateInfo = {};
		InputAssemblyStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		InputAssemblyStateInfo.topology = PrimitiveTopologyToVkPrimitiveTopology(InCreateInfo.PrimitiveTopology);

		VkPipelineViewportStateCreateInfo ViewportStateInfo = {};
		ViewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		ViewportStateInfo.scissorCount = 1;
		ViewportStateInfo.viewportCount = 1;

		VkPipelineRasterizationStateCreateInfo RasterizationStateInfo = {};
		RasterizationStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		RasterizationStateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		RasterizationStateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		RasterizationStateInfo.frontFace = InCreateInfo.WindingOrder == EWindingOrder::CounterClockwise ? VK_FRONT_FACE_COUNTER_CLOCKWISE : VK_FRONT_FACE_CLOCKWISE;
		RasterizationStateInfo.depthBiasEnable = VK_TRUE;
		RasterizationStateInfo.depthClampEnable = VK_FALSE;
		RasterizationStateInfo.depthBiasConstantFactor = 0.0f;
		RasterizationStateInfo.depthBiasSlopeFactor = 0.0f;
		RasterizationStateInfo.rasterizerDiscardEnable = VK_FALSE;
		RasterizationStateInfo.lineWidth = 1.0f;

		VkPipelineMultisampleStateCreateInfo MultisampleStateInfo = {};
		MultisampleStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		MultisampleStateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		MultisampleStateInfo.sampleShadingEnable = VK_FALSE;

		VkPipelineDepthStencilStateCreateInfo DepthStencilStateInfo = {};
		DepthStencilStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		DepthStencilStateInfo.flags = 0;
		DepthStencilStateInfo.depthTestEnable = IsDepthFormat(InCreateInfo.DepthStencilFormat);
		DepthStencilStateInfo.depthWriteEnable = VK_TRUE;
		DepthStencilStateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		DepthStencilStateInfo.depthBoundsTestEnable = VK_FALSE;
		DepthStencilStateInfo.stencilTestEnable = VK_FALSE;
		DepthStencilStateInfo.front.writeMask = 0xFF;
		DepthStencilStateInfo.front.compareMask = 0xFF;
		DepthStencilStateInfo.front.failOp = VK_STENCIL_OP_KEEP;
		DepthStencilStateInfo.front.depthFailOp = VK_STENCIL_OP_KEEP;
		DepthStencilStateInfo.front.passOp = VK_STENCIL_OP_KEEP;
		DepthStencilStateInfo.front.compareOp = VK_COMPARE_OP_ALWAYS;
		DepthStencilStateInfo.back.writeMask = 0xFF;
		DepthStencilStateInfo.back.compareMask = 0xFF;
		DepthStencilStateInfo.back.failOp = VK_STENCIL_OP_KEEP;
		DepthStencilStateInfo.back.depthFailOp = VK_STENCIL_OP_KEEP;
		DepthStencilStateInfo.back.passOp = VK_STENCIL_OP_KEEP;
		DepthStencilStateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
		DepthStencilStateInfo.minDepthBounds = 0.0f;
		DepthStencilStateInfo.maxDepthBounds = 0.0f;

		VkPipelineColorBlendStateCreateInfo ColorBlendStateInfo = {};
		ColorBlendStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		/*ColorBlendStateInfo.logicOpEnable = VK_TRUE;
		ColorBlendStateInfo.logicOp = VK_LOGIC_OP_CLEAR;*/

		std::vector<VkPipelineColorBlendAttachmentState> AttachmentBlendStates;

		for (uint32_t Idx = 0; Idx < InCreateInfo.RenderTargetCount; Idx++)
		{
			auto& AttachmentBlendState = AttachmentBlendStates.emplace_back();
			AttachmentBlendState.blendEnable = InCreateInfo.RenderTargetBlendStates[Idx].EnableBlending ? VK_TRUE : VK_FALSE;
			AttachmentBlendState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			AttachmentBlendState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			AttachmentBlendState.colorBlendOp = VK_BLEND_OP_ADD;
			AttachmentBlendState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			AttachmentBlendState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			AttachmentBlendState.alphaBlendOp = VK_BLEND_OP_ADD;
			AttachmentBlendState.colorWriteMask = 0xF;
		}

		ColorBlendStateInfo.attachmentCount = AttachmentBlendStates.size();
		ColorBlendStateInfo.pAttachments = AttachmentBlendStates.data();

		auto DynamicStates = std::array { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

		VkPipelineDynamicStateCreateInfo DynamicStateInfo = {};
		DynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		DynamicStateInfo.dynamicStateCount = DynamicStates.size();
		DynamicStateInfo.pDynamicStates = DynamicStates.data();

		VkPipelineRenderingCreateInfoKHR RenderingCreateInfoKHR = {};
		RenderingCreateInfoKHR.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;

		std::vector<VkFormat> ColorAttachmentFormats;
		for (uint32_t Idx = 0; Idx < InCreateInfo.RenderTargetCount; Idx++)
			ColorAttachmentFormats.push_back(EFormatToVkFormat(InCreateInfo.RenderTargetFormats[Idx]));

		RenderingCreateInfoKHR.colorAttachmentCount = ColorAttachmentFormats.size();
		RenderingCreateInfoKHR.pColorAttachmentFormats = ColorAttachmentFormats.data();
		RenderingCreateInfoKHR.depthAttachmentFormat = EFormatToVkFormat(InCreateInfo.DepthStencilFormat);
		RenderingCreateInfoKHR.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

		VkGraphicsPipelineCreateInfo PipelineCreateInfo = {};
		PipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		PipelineCreateInfo.pNext = &RenderingCreateInfoKHR;
		PipelineCreateInfo.flags = 0;
		PipelineCreateInfo.stageCount = ShaderStageCreateInfos.size();
		PipelineCreateInfo.pStages = ShaderStageCreateInfos.data();
		PipelineCreateInfo.pVertexInputState = &VertexInputInfo;
		PipelineCreateInfo.pInputAssemblyState = &InputAssemblyStateInfo;
		PipelineCreateInfo.pViewportState = &ViewportStateInfo;
		PipelineCreateInfo.pRasterizationState = &RasterizationStateInfo;
		PipelineCreateInfo.pMultisampleState = &MultisampleStateInfo;
		PipelineCreateInfo.pDepthStencilState = &DepthStencilStateInfo;
		PipelineCreateInfo.pColorBlendState = &ColorBlendStateInfo;
		PipelineCreateInfo.pDynamicState = &DynamicStateInfo;
		PipelineCreateInfo.layout = m_Layout;
		PipelineCreateInfo.subpass = 0;
		PipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		PipelineCreateInfo.basePipelineIndex = 0;

		vkCreateGraphicsPipelines(InContext->GetDevice()->GetLogicalDevice(), VK_NULL_HANDLE, 1, &PipelineCreateInfo, nullptr, &m_Pipeline);
	}

	void GraphicsPipeline::Bind(CommandBuffer* InCmdList)
	{
		vkCmdBindPipeline(InCmdList->GetBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
	}

}
