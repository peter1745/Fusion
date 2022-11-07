#pragma once

#include "Fusion/Core/Enum.hpp"

#include <variant>

namespace Fusion {

	using EPipelineLayoutDescriptorRangeFlags = Flags<>;
	using EPipelineLayoutFlags = Flags<>;

	namespace DescriptorRangeFlags
	{
		static constexpr EPipelineLayoutDescriptorRangeFlags None = (1 << 0);
		static constexpr EPipelineLayoutDescriptorRangeFlags VolatileDescriptor = (1 << 1);
		static constexpr EPipelineLayoutDescriptorRangeFlags VolatileData = (1 << 2);
		static constexpr EPipelineLayoutDescriptorRangeFlags StaticData = (1 << 3);
	}

	namespace PipelineLayoutFlags
	{
		static constexpr EPipelineLayoutFlags None = (1 << 0);
		static constexpr EPipelineLayoutFlags AllowInputAssemblerInputLayout = (1 << 1);
	}

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
		EPipelineLayoutDescriptorRangeFlags Flags;
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

	enum class EFilterMode
	{
		Nearest, Linear
	};

	enum class EImageAddressMode { Wrap, Mirror, Clamp, Border, MirrorOnce };

	struct PipelineStaticSampler
	{
		EFilterMode MinFilter;
		EFilterMode MagFilter;
		EImageAddressMode AddressModeU;
		EImageAddressMode AddressModeV;
		EImageAddressMode AddressModeW;
		uint32_t Binding;
		uint32_t Register;
		EShaderVisibility Visibility;
	};

	struct PipelineLayoutInfo
	{
		std::vector<PipelineLayoutParameter> Parameters;
		std::vector<PipelineStaticSampler> StaticSamplers;
		EPipelineLayoutFlags Flags;
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
