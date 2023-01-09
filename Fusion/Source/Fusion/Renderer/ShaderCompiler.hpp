#pragma once

#include "Device.hpp"
#include "ShaderCompiler.hpp"
#include "CommonTypes.hpp"

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

	struct CompiledShaderData
	{
		std::unordered_map<EShaderType, std::vector<uint32_t>> ModuleByteCodes;
		std::unordered_map<EShaderType, VulkanModuleReflectionData> ReflectionData;
	};

	class Shader;

	class ShaderCompiler
	{
	public:
		Shared<Shader> CreateShader(const Shared<Device>& InDevice, const std::filesystem::path& InFilePath);

	private:
		void TryCompileAndReflectModule(const std::filesystem::path& InFilePath, EShaderType InShaderType, CompiledShaderData& OutData);
		shaderc::SpvCompilationResult TryCompileModule(const std::filesystem::path& InFilePath, EShaderType InShaderType);
		void ReflectShader(const std::vector<uint32_t>& InByteCode, VulkanModuleReflectionData& OutReflectionData);
	};

}
