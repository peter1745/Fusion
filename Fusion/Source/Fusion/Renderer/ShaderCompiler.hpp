#pragma once

#include "Device.hpp"
#include "ShaderCompiler.hpp"
#include "CommonTypes.hpp"

#include <shaderc/shaderc.hpp>

namespace Fusion {

	enum class EIOParameterType { Variable, Struct };

	struct ShaderIOParameter
	{
		EShaderStage Stage;
		std::string Name;
		uint32_t Location;
		EIOParameterType Type;
		EFormat Format;
	};

	struct UniformInfo
	{
		std::string Name;
		uint32_t Offset;
		uint32_t Size;
	};

	struct PushConstantData
	{
		EShaderStage Stages;
		std::string Name;
		uint32_t Size;

		std::vector<UniformInfo> Uniforms;
	};

	enum class EShaderResourceType
	{
		UniformBuffer,
		CombinedImageSampler,
		Count
	};

	struct ShaderResource
	{
		EShaderStage Stages;
		EShaderResourceType Type;
		std::string Name;
		uint32_t Binding;
	};

	struct UniformResource
	{
		std::string Name;
		EFormat Format;
		uint32_t Size;
		uint32_t Offset;
	};

	struct UniformBufferShaderResource : public ShaderResource
	{
		uint32_t Size;
		std::vector<UniformResource> Uniforms;
	};

	struct SamplerShaderResource : public ShaderResource
	{
		uint32_t Dimensions;
	};

	struct ShaderDescriptorSet
	{
		uint32_t Index;
		std::unordered_map<uint32_t, ShaderResource*> Resources;
	};

	struct CompiledShaderData
	{
		std::unordered_map<EShaderStage, std::vector<uint32_t>> ModuleByteCodes;

		std::vector<ShaderIOParameter> InputParameters;
		// NOTE(Peter): Even though Vulkan currently only allows one push constant per shader module
		//              SPIRV-Cross offers us an array in case that restriction is lifted at some point
		//              so we'll also support it
		std::vector<PushConstantData> PushConstants;
		std::vector<ShaderDescriptorSet> DescriptorSets;
		std::vector<ShaderIOParameter> OutputParameters;
	};

	class Shader;

	class ShaderCompiler
	{
	public:
		Shared<Shader> CompileFromFile(const std::filesystem::path& InFilePath);

	private:
		void TryCompileAndReflectModule(const std::filesystem::path& InFilePath, EShaderStage InShaderType, CompiledShaderData& OutData);
		shaderc::SpvCompilationResult TryCompileModule(const std::filesystem::path& InFilePath, EShaderStage InShaderType);
		void ReflectShader(EShaderStage InStage, CompiledShaderData& InCompiledData);

		void PrintReflectionData(const CompiledShaderData& InShaderData) const;
	};

}
