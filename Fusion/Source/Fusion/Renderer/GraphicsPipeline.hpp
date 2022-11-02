#pragma once

#include "PipelineLayout.hpp"
#include "Shader.hpp"

#include <array>

namespace Fusion {

	enum class EPrimitiveTopology
	{
		Triangles
	};

	enum class EWindingOrder { Clockwise, CounterClockwise };

	static constexpr uint32_t AppendAlignedElement = 0xFFFFFFFF;

	struct GraphicsPipelineInput
	{
		std::string Name;
		uint32_t Index;
		EFormat Format;
		uint32_t Binding;
		uint32_t Offset;
		uint32_t InstanceStep;
	};

	struct GraphicsPipelineInfo
	{
		PipelineLayout* Layout;
		std::vector<GraphicsPipelineInput> Inputs;

		Shared<Shader> PipelineShader;

		// Blend State

		EPrimitiveTopology PrimitiveTopology;
		EWindingOrder WindingOrder;

		uint8_t RenderTargetCount;
		std::array<EFormat, 8> RenderTargetFormats;
		EFormat DepthStencilFormat;
	};

	class GraphicsPipeline : public SharedObject
	{
	public:
		virtual ~GraphicsPipeline() = default;

		virtual void Bind() = 0;

		virtual const GraphicsPipelineInfo& GetInfo() const = 0;

	public:
		static Unique<GraphicsPipeline> Create(const GraphicsPipelineInfo& InCreateInfo);
	};

}
