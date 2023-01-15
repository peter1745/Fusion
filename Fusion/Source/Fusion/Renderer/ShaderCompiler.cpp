#include "FusionPCH.hpp"
#include "ShaderCompiler.hpp"
#include "Shader.hpp"

#include "Fusion/IO/FileIO.hpp"

#include <spirv_cross/spirv_glsl.hpp>

namespace Fusion {

	static std::unordered_map<EShaderType, const char*> s_ShaderNames = {
		{ EShaderType::Vertex, "vertex" },
		{ EShaderType::Pixel, "pixel" }
	};

	static std::unordered_map<EShaderType, shaderc_shader_kind> s_ShaderTypeToShaderCShaderKind = {
		{ EShaderType::Vertex, shaderc_vertex_shader },
		{ EShaderType::Pixel, shaderc_fragment_shader }
	};

	static std::unordered_map<EShaderType, const char*> s_ShaderEntryPoints = {
		{ EShaderType::Vertex, "VertexMain" },
		{ EShaderType::Pixel, "PixelMain" }
	};

	static std::unordered_map<spirv_cross::SPIRType::BaseType, std::string> s_SPIRTypeName = {
		{ spirv_cross::SPIRType::BaseType::Unknown, "Unknown" },
		{ spirv_cross::SPIRType::BaseType::Void, "Void" },
		{ spirv_cross::SPIRType::BaseType::Boolean, "Boolean" },
		{ spirv_cross::SPIRType::BaseType::SByte, "SByte" },
		{ spirv_cross::SPIRType::BaseType::UByte, "UByte" },
		{ spirv_cross::SPIRType::BaseType::Short, "Short" },
		{ spirv_cross::SPIRType::BaseType::UShort, "UShort" },
		{ spirv_cross::SPIRType::BaseType::Int, "Int" },
		{ spirv_cross::SPIRType::BaseType::UInt, "UInt" },
		{ spirv_cross::SPIRType::BaseType::Int64, "Int64" },
		{ spirv_cross::SPIRType::BaseType::UInt64, "UInt64" },
		{ spirv_cross::SPIRType::BaseType::AtomicCounter, "AtomicCounter" },
		{ spirv_cross::SPIRType::BaseType::Half, "Half" },
		{ spirv_cross::SPIRType::BaseType::Float, "Float" },
		{ spirv_cross::SPIRType::BaseType::Double, "Double" },
		{ spirv_cross::SPIRType::BaseType::Struct, "Struct" },
		{ spirv_cross::SPIRType::BaseType::Image, "Image" },
		{ spirv_cross::SPIRType::BaseType::SampledImage, "SampledImage" },
		{ spirv_cross::SPIRType::BaseType::Sampler, "Sampler" },
		{ spirv_cross::SPIRType::BaseType::AccelerationStructure, "AccelerationStructure" },
		{ spirv_cross::SPIRType::BaseType::RayQuery, "RayQuery" },
	};

	static EFormat GetResourceFormat(spirv_cross::SPIRType InType)
	{
		switch (InType.basetype)
		{
		case spirv_cross::SPIRType::Unknown: return EFormat::Unknown;
		case spirv_cross::SPIRType::Void: return EFormat::Unknown;
		case spirv_cross::SPIRType::UByte:
		case spirv_cross::SPIRType::UShort:
		case spirv_cross::SPIRType::UInt:
		case spirv_cross::SPIRType::Boolean:
		{
			switch (InType.vecsize)
			{
			case 1: return EFormat::R32UInt;
			case 2: return EFormat::RG32UInt;
			case 3: return EFormat::RGB32UInt;
			case 4: return EFormat::RGBA32UInt;
			}

			break;
		}
		case spirv_cross::SPIRType::Int:
		case spirv_cross::SPIRType::Short:
		case spirv_cross::SPIRType::SByte:
		{
			switch (InType.vecsize)
			{
			case 1: return EFormat::R32SInt;
			case 2: return EFormat::RG32SInt;
			case 3: return EFormat::RGB32SInt;
			case 4: return EFormat::RGBA32SInt;
			}

			break;
		}
		case spirv_cross::SPIRType::Int64:
		{
			switch (InType.vecsize)
			{
			case 1: return EFormat::RG32SInt;
			case 2: return EFormat::RGBA32SInt;
			}

			break;
		}
		case spirv_cross::SPIRType::UInt64:
		{
			switch (InType.vecsize)
			{
			case 1: return EFormat::RG32UInt;
			case 2: return EFormat::RGBA32UInt;
			}

			break;
		}
		case spirv_cross::SPIRType::Float:
		{
			switch (InType.vecsize)
			{
			case 1: return EFormat::R32Float;
			case 2: return EFormat::RG32Float;
			case 3: return EFormat::RGB32Float;
			case 4: return EFormat::RGBA32Float;
			}

			break;
		}
		case spirv_cross::SPIRType::Double:
		{
			switch (InType.vecsize)
			{
			case 1: return EFormat::R32Float;
			case 2: return EFormat::RG32Float;
			case 3: return EFormat::RGB32Float;
			case 4: return EFormat::RGBA32Float;
			}

			break;
		}
		case spirv_cross::SPIRType::Struct:
		{
			// TODO
			break;
		}
		}

		CoreVerify(false);
		return EFormat::Unknown;
	}

	Shared<Shader> ShaderCompiler::CreateShader(const Shared<Device>& InDevice, const std::filesystem::path& InFilePath)
	{
		CompiledShaderData ShaderData = {};

		TryCompileAndReflectModule(InFilePath, EShaderType::Vertex, ShaderData);
		TryCompileAndReflectModule(InFilePath, EShaderType::Pixel, ShaderData);

		return Shared<Shader>::Create(InDevice, ShaderData);
	}

	void ShaderCompiler::TryCompileAndReflectModule(const std::filesystem::path& InFilePath, EShaderType InShaderType, CompiledShaderData& OutData)
	{
		shaderc::SpvCompilationResult Result = TryCompileModule(InFilePath, InShaderType);

		if (Result.GetCompilationStatus() != shaderc_compilation_status_success)
			return;

		OutData.ModuleByteCodes[InShaderType] = std::vector<uint32_t>(Result.begin(), Result.end());

		ReflectShader(OutData.ModuleByteCodes[InShaderType], OutData.ReflectionData[InShaderType]);

		if (OutData.ReflectionData.find(InShaderType) != OutData.ReflectionData.end())
		{
			const auto& ModuleReflectionInfo = OutData.ReflectionData.at(InShaderType);

			LogInfo("Fusion", "Input Parameters:");
			for (const auto& InputParam : ModuleReflectionInfo.InputParameters)
			{
				LogInfo("Fusion", "\tName: {}", InputParam.Name);
				LogInfo("Fusion", "\tLocation: {}", InputParam.Location);
				LogInfo("Fusion", "\t--------------------");
			}

			LogInfo("Fusion", "Push Constants:");
			for (const auto& PushConstant : ModuleReflectionInfo.PushConstants)
			{
				LogInfo("Fusion", "\tName: {}", PushConstant.Name);
				LogInfo("Fusion", "\tSize: {}", PushConstant.Size);
				LogInfo("Fusion", "\tUniforms:");

				for (const auto& Uniform : PushConstant.Uniforms)
				{
					LogInfo("Fusion", "\t\tName: {}", Uniform.Name);
					LogInfo("Fusion", "\t\tOffset: {}", Uniform.Offset);
					LogInfo("Fusion", "\t\tSize: {}", Uniform.Size);
					LogInfo("Fusion", "\t--------------------");
				}

				LogInfo("Fusion", "\t--------------------");
			}

			/*LogInfo(""Fusion", Resources:");
			for (const auto& ResourceInfo : ModuleReflectionInfo.Resources)
			{
				LogInfo("\"Fusion", tName: {}", ResourceInfo.Name);
				LogInfo("\"Fusion", tType: {}", s_ShaderInputTypeNames.at(static_cast<D3D_SHADER_INPUT_TYPE>(ResourceInfo.Type)));
				LogInfo("\"Fusion", tBinding Point: {}", ResourceInfo.BindingPoint);
				LogInfo("\"Fusion", tBinding Count: {}", ResourceInfo.BindingCount);
				LogInfo("\"Fusion", t--------------------");
			}*/

			LogInfo("Fusion", "Output Parameters:");
			for (const auto& OutputParam : ModuleReflectionInfo.OutputParameters)
			{
				LogInfo("Fusion", "\tName: {}", OutputParam.Name);
				LogInfo("Fusion", "\tLocation: {}", OutputParam.Location);
				LogInfo("Fusion", "\t--------------------");
			}
		}
	}

	shaderc::SpvCompilationResult ShaderCompiler::TryCompileModule(const std::filesystem::path& InFilePath, EShaderType InShaderType)
	{
		LogInfo("Fusion", "Compiling {} shader from {} for Vulkan", s_ShaderNames.at(InShaderType), InFilePath.string());

		std::string Source = "";
		if (!FileIO::ReadFileText(InFilePath, Source))
		{
			LogError("Fusion", "Failed! File not found.");
			return shaderc::SpvCompilationResult(nullptr);
		}

		shaderc::Compiler Compiler;
		shaderc::CompileOptions Options;
		Options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);

#ifdef FUSION_DEBUG
		Options.SetGenerateDebugInfo();
		Options.SetOptimizationLevel(shaderc_optimization_level_zero);
#else
		Options.SetOptimizationLevel(shaderc_optimization_level_performance);
#endif

		switch (InShaderType)
		{
		case EShaderType::Vertex:
		{
			Options.AddMacroDefinition("FUSION_COMPILE_VERTEX");
			break;
		}
		case EShaderType::Pixel:
		{
			Options.AddMacroDefinition("FUSION_COMPILE_PIXEL");
			break;
		}
		}

		Options.SetWarningsAsErrors();

		auto ShaderKind = s_ShaderTypeToShaderCShaderKind.at(InShaderType);
		std::string ShaderName = InFilePath.filename().string();
		shaderc::SpvCompilationResult Module = Compiler.CompileGlslToSpv(Source, ShaderKind, ShaderName.c_str(), Options);

		if (Module.GetCompilationStatus() != shaderc_compilation_status_success)
			LogError("Fusion", "Failed! Error: {}", Module.GetErrorMessage());

		return Module;
	}

	void ShaderCompiler::ReflectShader(const std::vector<uint32_t>& InByteCode, VulkanModuleReflectionData& OutReflectionData)
	{
		LogInfo("Fusion", "Reflecting for Vulkan");

		spirv_cross::CompilerGLSL Compiler(InByteCode);

		spirv_cross::ShaderResources Resources = Compiler.get_shader_resources();

		for (const auto& InputResource : Resources.stage_inputs)
		{
			const auto& Type = Compiler.get_type(InputResource.base_type_id);

			if (Type.basetype == spirv_cross::SPIRType::Struct)
			{
				auto MemberCount = uint32_t(Type.member_types.size());

				for (uint32_t MemberIdx = 0; MemberIdx < MemberCount; MemberIdx++)
				{
					auto& InputParam = OutReflectionData.InputParameters.emplace_back();

					const auto& MemberType = Compiler.get_type(Type.member_types[MemberIdx]);
					InputParam.Name = Compiler.get_member_name(Type.self, MemberIdx);
					InputParam.Location = MemberIdx;
					InputParam.Format = GetResourceFormat(MemberType);
				}
			}
			else
			{
				auto& InputParam = OutReflectionData.InputParameters.emplace_back();
				InputParam.Name = InputResource.name;
				InputParam.Location = Compiler.get_decoration(InputResource.id, spv::DecorationLocation);
				InputParam.Format = GetResourceFormat(Type);
			}
		}

		for (const auto& BufferResource : Resources.uniform_buffers)
		{
			LogInfo("Fusion", BufferResource.name);
		}

		for (const auto& PushConstantResource : Resources.push_constant_buffers)
		{
			auto& PushConstant = OutReflectionData.PushConstants.emplace_back();
			const auto& BufferType = Compiler.get_type(PushConstantResource.base_type_id);

			PushConstant.Size = Compiler.get_declared_struct_size(BufferType);

			auto MemberCount = uint32_t(BufferType.member_types.size());
			PushConstant.Uniforms.resize(MemberCount);

			for (uint32_t MemberIdx = 0; MemberIdx < MemberCount; MemberIdx++)
			{
				auto& MemberInfo = PushConstant.Uniforms[MemberIdx];

				const auto& MemberType = Compiler.get_type(BufferType.member_types[MemberIdx]);
				MemberInfo.Name = Compiler.get_member_name(BufferType.self, MemberIdx);
				MemberInfo.Size = Compiler.get_declared_struct_member_size(BufferType, MemberIdx);
				MemberInfo.Offset = Compiler.type_struct_member_offset(BufferType, MemberIdx);
			}
		}

		for (const auto& OutputResource : Resources.stage_outputs)
		{
			const auto& Type = Compiler.get_type(OutputResource.base_type_id);

			if (Type.basetype == spirv_cross::SPIRType::Struct)
			{
				auto MemberCount = uint32_t(Type.member_types.size());

				for (uint32_t MemberIdx = 0; MemberIdx < MemberCount; MemberIdx++)
				{
					auto& OutputParam = OutReflectionData.OutputParameters.emplace_back();

					const auto& MemberType = Compiler.get_type(Type.member_types[MemberIdx]);
					OutputParam.Name = Compiler.get_member_name(Type.self, MemberIdx);
					OutputParam.Location = MemberIdx;
					OutputParam.Format = GetResourceFormat(MemberType);
				}
			}
			else
			{
				auto& OutputParam = OutReflectionData.OutputParameters.emplace_back();
				OutputParam.Name = OutputResource.name;
				OutputParam.Location = Compiler.get_decoration(OutputResource.id, spv::DecorationLocation);
				OutputParam.Format = GetResourceFormat(Type);
			}
		}
	}
}