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

	struct ResourceInfo
	{
		std::string Name;
		uint32_t BindingPoint;
		EShaderVisibility Visibility;
	};

	class GraphicsPipeline : public SharedObject
	{
	public:
		GraphicsPipeline(const Shared<GraphicsContext>& InContext, const GraphicsPipelineInfo& InCreateInfo);

		void Bind(CommandBuffer* InCmdList);

		const ResourceInfo& GetResourceInfo(const std::string& InName) const { return m_Resources.at(InName); }
		const GraphicsPipelineInfo& GetInfo() const { return m_CreateInfo; }

		VkPipelineLayout GetPipelineLayout() const { return m_Layout; }

	private:
		GraphicsPipelineInfo m_CreateInfo;

		VkPipelineLayout m_Layout;
		VkPipeline m_Pipeline;

		std::unordered_map<std::string, ResourceInfo> m_Resources;

	};

}
