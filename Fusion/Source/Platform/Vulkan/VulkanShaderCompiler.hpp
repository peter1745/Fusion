#pragma once

#include "Fusion/Renderer/ShaderCompiler.hpp"

#include <shaderc/shaderc.hpp>

namespace Fusion {

	enum class EIOParameterType { Variable, Struct };

	struct ShaderIOParameter
	{
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
		std::string Name;
		uint32_t Size;

		std::vector<UniformInfo> Uniforms;
	};

	struct VulkanModuleReflectionData
	{
		std::vector<ShaderIOParameter> InputParameters;
		// NOTE(Peter): Even though Vulkan currently only allows one push constant per shader module
		//              SPIRV-Cross offers us an array in case that restriction is lifted at some point
		//              so we'll also support it
		std::vector<PushConstantData> PushConstants;
		std::vector<ShaderIOParameter> OutputParameters;
	};

	struct VulkanCompiledShaderData
	{
		std::unordered_map<EShaderType, std::vector<uint32_t>> ModuleByteCodes;
		std::unordered_map<EShaderType, VulkanModuleReflectionData> ReflectionData;
	};

	class VulkanShaderCompiler : public ShaderCompiler
	{
	public:
		Shared<Shader> CreateShader(const std::filesystem::path& InFilePath) override;

	private:
		void TryCompileAndReflectModule(const std::filesystem::path& InFilePath, EShaderType InShaderType, VulkanCompiledShaderData& OutData);
		shaderc::SpvCompilationResult TryCompileModule(const std::filesystem::path& InFilePath, EShaderType InShaderType);
		void ReflectShader(const std::vector<uint32_t>& InByteCode, VulkanModuleReflectionData& OutReflectionData);
	};

}
