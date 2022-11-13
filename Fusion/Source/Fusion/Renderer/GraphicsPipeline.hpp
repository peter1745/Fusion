#pragma once

#include "CommandList.hpp"
#include "Shader.hpp"

#include <array>

namespace Fusion {

	enum class EPrimitiveTopology
	{
		Triangles
	};

	enum class EWindingOrder { Clockwise, CounterClockwise };

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
		EFormat DepthStencilFormat;
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
		virtual ~GraphicsPipeline() = default;

		virtual void Bind(CommandList* InCmdList) = 0;

		virtual const ResourceInfo& GetResourceInfo(const std::string& InName) const = 0;

		virtual const GraphicsPipelineInfo& GetInfo() const = 0;

	public:
		static Unique<GraphicsPipeline> Create(const GraphicsPipelineInfo& InCreateInfo);
	};

}
