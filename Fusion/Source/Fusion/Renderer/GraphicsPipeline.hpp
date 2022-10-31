#pragma once

#include "PipelineLayout.hpp"
#include "Shader.hpp"

#include <array>

namespace Fusion {

	enum class EGraphicsFormat
	{
		Unknown,
		RGBA32Float, RGBA32UInt,
		RGB32Float, RGB32UInt,
		RG32Float, RG32UInt,
		R32Float, R32UInt,

		RGBA8Unorm, RGBA8UInt,

		D24UnormS8UInt
	};

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
		EGraphicsFormat Format;
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
		std::array<EGraphicsFormat, 8> RenderTargetFormats;
		EGraphicsFormat DepthStencilFormat;
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
