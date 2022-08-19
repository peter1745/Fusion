#include "FusionPCH.h"
#include "VulkanPipeline.h"

namespace Fusion {

	static VkFormat ShaderDataTypeToVkFormat(ShaderDataType InType)
	{
		switch (InType)
		{
		case ShaderDataType::Float: return VK_FORMAT_R32_SFLOAT;
		case ShaderDataType::Float2: return VK_FORMAT_R32G32_SFLOAT;
		case ShaderDataType::Float3: return VK_FORMAT_R32G32B32_SFLOAT;
		case ShaderDataType::Float4: return VK_FORMAT_R32G32B32A32_SFLOAT;
		/*case ShaderDataType::Mat3x3: return VK_FORMAT_R32_SFLOAT;
		case ShaderDataType::Mat4x4: return VK_FORMAT_R32_SFLOAT;*/
		}
		
		FUSION_CORE_VERIFY(false);
		return VK_FORMAT_UNDEFINED;
	}

	VulkanPipeline::VulkanPipeline(const PipelineSpecification& InSpecification, const Shared<VulkanDevice>& InDevice)
		: m_Specification(InSpecification), m_Device(InDevice)
	{
		// Setup dynamic states (useful for viewport and scissors, maybe even blend states)
		VkDynamicState DynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		VkPipelineDynamicStateCreateInfo DynamicStateCreateInfo = {};
		DynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		DynamicStateCreateInfo.dynamicStateCount = sizeof(DynamicStates) / sizeof(VkDynamicState);
		DynamicStateCreateInfo.pDynamicStates = DynamicStates;

		// Vertex Input State
		VkVertexInputBindingDescription VertexInputDescription = {};
		VertexInputDescription.binding = 0;
		VertexInputDescription.stride = InSpecification.Layout.GetStride();
		VertexInputDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		const auto& VertexLayoutAttributes = InSpecification.Layout.GetAttributes();
		std::vector<VkVertexInputAttributeDescription> VertexAttributeDescriptions(VertexLayoutAttributes.size());
		for (uint32_t i = 0; i < VertexLayoutAttributes.size(); i++)
		{
			VertexAttributeDescriptions[i].binding = 0;
			VertexAttributeDescriptions[i].location = VertexLayoutAttributes[i].Location;
			VertexAttributeDescriptions[i].format = ShaderDataTypeToVkFormat(VertexLayoutAttributes[i].Type);
			VertexAttributeDescriptions[i].offset = VertexLayoutAttributes[i].Offset;
		}

		VkPipelineVertexInputStateCreateInfo VertexInputStateCreateInfo = {};
		VertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		VertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
		VertexInputStateCreateInfo.pVertexBindingDescriptions = &VertexInputDescription;
		VertexInputStateCreateInfo.vertexAttributeDescriptionCount = VertexAttributeDescriptions.size();
		VertexInputStateCreateInfo.pVertexAttributeDescriptions = VertexAttributeDescriptions.data();

		VkPipelineInputAssemblyStateCreateInfo InputAssemblyStateCreateInfo = {};
		InputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		InputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		InputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo ViewportStateCreateInfo = {};
		ViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		ViewportStateCreateInfo.viewportCount = 1;
		ViewportStateCreateInfo.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo RasterizationStateCreateInfo = {};
		RasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		RasterizationStateCreateInfo.depthClampEnable = VK_FALSE; // This would be VK_TRUE for e.g shadow maps
		RasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		// Polygon mode other than fill requires enabling a GPU feature
		RasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		// Line width >1.0f requires enabling a GPU feature
		RasterizationStateCreateInfo.lineWidth = 1.0f;
		RasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		RasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		RasterizationStateCreateInfo.depthBiasEnable = VK_FALSE; // Might be VK_TRUE for shadow maps

		// For now keep multisampling disabled
		VkPipelineMultisampleStateCreateInfo MultiSampleStateCreateInfo = {};
		MultiSampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		MultiSampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
		MultiSampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendAttachmentState ColorBlendAttachmentState = {};
		ColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		ColorBlendAttachmentState.blendEnable = VK_TRUE;
		ColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		ColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		ColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
		ColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		ColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		ColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo ColorBlendAttachmentCreateInfo = {};
		ColorBlendAttachmentCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		ColorBlendAttachmentCreateInfo.logicOpEnable = VK_FALSE;
		ColorBlendAttachmentCreateInfo.logicOp = VK_LOGIC_OP_COPY;
		ColorBlendAttachmentCreateInfo.attachmentCount = 1;
		ColorBlendAttachmentCreateInfo.pAttachments = &ColorBlendAttachmentState;
		ColorBlendAttachmentCreateInfo.blendConstants[0] = 0.0f;
		ColorBlendAttachmentCreateInfo.blendConstants[1] = 0.0f;
		ColorBlendAttachmentCreateInfo.blendConstants[2] = 0.0f;
		ColorBlendAttachmentCreateInfo.blendConstants[3] = 0.0f;

		VkPipelineLayoutCreateInfo LayoutCreateInfo = {};
		LayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		LayoutCreateInfo.setLayoutCount = 0;
		LayoutCreateInfo.pSetLayouts = nullptr;
		LayoutCreateInfo.pushConstantRangeCount = 0;
		LayoutCreateInfo.pPushConstantRanges = nullptr;

		FUSION_CORE_VERIFY(vkCreatePipelineLayout(InDevice->GetLogicalDevice(), &LayoutCreateInfo, nullptr, &m_Layout) == VK_SUCCESS);

		VkPipelineRenderingCreateInfo DynamicRenderingCreateInfo = {};
		DynamicRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		DynamicRenderingCreateInfo.colorAttachmentCount = 1;
		DynamicRenderingCreateInfo.pColorAttachmentFormats = &m_Specification.ColorAttachmentFormat;
		DynamicRenderingCreateInfo.depthAttachmentFormat = VK_FORMAT_UNDEFINED;
		DynamicRenderingCreateInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

		// Create the pipeline
		VkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo = {};
		GraphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		GraphicsPipelineCreateInfo.stageCount = VulkanShader::MaxShaderTypes;
		GraphicsPipelineCreateInfo.pStages = InSpecification.PipelineShader->GetShaderStages().data();
		GraphicsPipelineCreateInfo.pVertexInputState = &VertexInputStateCreateInfo;
		GraphicsPipelineCreateInfo.pInputAssemblyState = &InputAssemblyStateCreateInfo;
		GraphicsPipelineCreateInfo.pViewportState = &ViewportStateCreateInfo;
		GraphicsPipelineCreateInfo.pRasterizationState = &RasterizationStateCreateInfo;
		GraphicsPipelineCreateInfo.pMultisampleState = &MultiSampleStateCreateInfo;
		GraphicsPipelineCreateInfo.pDepthStencilState = nullptr; // TODO
		GraphicsPipelineCreateInfo.pColorBlendState = &ColorBlendAttachmentCreateInfo;
		GraphicsPipelineCreateInfo.pDynamicState = &DynamicStateCreateInfo;
		GraphicsPipelineCreateInfo.layout = m_Layout;
		GraphicsPipelineCreateInfo.renderPass = VK_NULL_HANDLE;
		GraphicsPipelineCreateInfo.subpass = 0;
		GraphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		GraphicsPipelineCreateInfo.basePipelineIndex = -1;
		GraphicsPipelineCreateInfo.pNext = &DynamicRenderingCreateInfo;

		FUSION_CORE_VERIFY(vkCreateGraphicsPipelines(InDevice->GetLogicalDevice(), VK_NULL_HANDLE, 1, &GraphicsPipelineCreateInfo, nullptr, &m_Pipeline) == VK_SUCCESS);
	}

	VulkanPipeline::~VulkanPipeline()
	{
		vkDestroyPipeline(m_Device->GetLogicalDevice(), m_Pipeline, nullptr);
		vkDestroyPipelineLayout(m_Device->GetLogicalDevice(), m_Layout, nullptr);
	}

}
