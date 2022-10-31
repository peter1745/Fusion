#pragma once

#include "Fusion/Core/Enum.hpp"

#include <variant>

namespace Fusion {

	enum class EPipelineLayoutDescriptorRangeFlags
	{
		None,
		VolatileDescriptor,
		VolatileData,
		StaticData
	};

	enum class EPipelineLayoutFlags { None, AllowInputAssemblerInputLayout };

	enum class EPipelineParameterType
	{
		DescriptorTable,
		Constants,
		ConstantBufferView,
		ShaderResourceView,
		UnorderedAccessView
	};

	enum class EShaderVisibility
	{
		All,
		Vertex,
		Pixel
	};

	struct PipelineLayoutDescriptor
	{
		uint32_t Binding;
		uint32_t Space;
	};

	struct PipelineLayoutConstants
	{
		uint32_t Binding;
		uint32_t Space;
		uint32_t Num32BitValues;
	};

	enum class EPipelineLayoutDescriptorRangeType
	{
		ConstantBufferView,
		ShaderResourceView,
		UnorderedAccessview,
		Sampler
	};

	struct PipelineLayoutDescriptorRange
	{
		EPipelineLayoutDescriptorRangeType Type;
		uint32_t NumDescriptors;
		uint32_t Binding;
		uint32_t Space;
		uint32_t Offset;
		Enum<EPipelineLayoutDescriptorRangeFlags> Flags;
	};

	struct PipelineLayoutDescriptorTable
	{
		std::vector<PipelineLayoutDescriptorRange> Ranges;
	};

	struct PipelineLayoutParameter
	{
		EPipelineParameterType Type;
		EShaderVisibility Visibility;
		std::variant<PipelineLayoutDescriptor, PipelineLayoutConstants, PipelineLayoutDescriptorTable> Value;
	};

	struct PipelineLayoutInfo
	{
		std::vector<PipelineLayoutParameter> Parameters;
		Enum<EPipelineLayoutFlags> Flags;
	};

	class PipelineLayout
	{
	public:
		virtual ~PipelineLayout() = default;

		virtual const PipelineLayoutInfo& GetInfo() const = 0;

	public:
		static Unique<PipelineLayout> Create(const PipelineLayoutInfo& InCreateInfo);
	};

}
