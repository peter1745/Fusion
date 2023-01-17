#include "FusionPCH.hpp"
#include "ShaderCompiler.hpp"
#include "Shader.hpp"

#include "Fusion/IO/FileIO.hpp"

#include <spirv_cross/spirv_glsl.hpp>

#include <ranges>

namespace Fusion {

	static std::unordered_map<EShaderStage, const char*> s_ShaderNames = {
		{ EShaderStage::Vertex, "Vertex" },
		{ EShaderStage::Pixel, "Pixel" }
	};

	static std::unordered_map<EShaderStage, shaderc_shader_kind> s_ShaderTypeToShaderCShaderKind = {
		{ EShaderStage::Vertex, shaderc_vertex_shader },
		{ EShaderStage::Pixel, shaderc_fragment_shader }
	};

	static std::unordered_map<EShaderResourceType, const char*> s_ShaderResourceTypeNames = {
		{ EShaderResourceType::CombinedImageSampler, "CombinedImageSampler" }
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

		TryCompileAndReflectModule(InFilePath, EShaderStage::Vertex, ShaderData);
		TryCompileAndReflectModule(InFilePath, EShaderStage::Pixel, ShaderData);

		PrintReflectionData(ShaderData);

		return Shared<Shader>::Create(InDevice, ShaderData);
	}

	void ShaderCompiler::TryCompileAndReflectModule(const std::filesystem::path& InFilePath, EShaderStage InShaderType, CompiledShaderData& OutData)
	{
		shaderc::SpvCompilationResult Result = TryCompileModule(InFilePath, InShaderType);

		if (Result.GetCompilationStatus() != shaderc_compilation_status_success)
			return;

		OutData.ModuleByteCodes[InShaderType] = std::vector<uint32_t>(Result.begin(), Result.end());

		ReflectShader(InShaderType, OutData);
	}

	shaderc::SpvCompilationResult ShaderCompiler::TryCompileModule(const std::filesystem::path& InFilePath, EShaderStage InShaderType)
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
		case EShaderStage::Vertex:
		{
			Options.AddMacroDefinition("FUSION_COMPILE_VERTEX");
			break;
		}
		case EShaderStage::Pixel:
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

	void ShaderCompiler::ReflectShader(EShaderStage InStage, CompiledShaderData& InCompiledData)
	{
		LogInfo("Fusion", "Reflecting for Vulkan");

		spirv_cross::CompilerGLSL Compiler(InCompiledData.ModuleByteCodes.at(InStage));

		spirv_cross::ShaderResources Resources = Compiler.get_shader_resources();

		for (const auto& InputResource : Resources.stage_inputs)
		{
			const auto& Type = Compiler.get_type(InputResource.base_type_id);

			if (Type.basetype == spirv_cross::SPIRType::Struct)
			{
				auto MemberCount = uint32_t(Type.member_types.size());

				for (uint32_t MemberIdx = 0; MemberIdx < MemberCount; MemberIdx++)
				{
					auto& InputParam = InCompiledData.InputParameters.emplace_back();

					const auto& MemberType = Compiler.get_type(Type.member_types[MemberIdx]);
					InputParam.Stage = InStage;
					InputParam.Name = Compiler.get_member_name(Type.self, MemberIdx);
					InputParam.Location = MemberIdx;
					InputParam.Format = GetResourceFormat(MemberType);
				}
			}
			else
			{
				auto& InputParam = InCompiledData.InputParameters.emplace_back();
				InputParam.Stage = InStage;
				InputParam.Name = InputResource.name;
				InputParam.Location = Compiler.get_decoration(InputResource.id, spv::DecorationLocation);
				InputParam.Format = GetResourceFormat(Type);
			}
		}

		for (const auto& SamplerResource : Resources.sampled_images)
		{
			uint32_t DescriptorSetIndex = Compiler.get_decoration(SamplerResource.id, spv::DecorationDescriptorSet);

			if (DescriptorSetIndex >= InCompiledData.DescriptorSets.size())
				InCompiledData.DescriptorSets.resize(DescriptorSetIndex + 1);

			auto& DescriptorSet = InCompiledData.DescriptorSets[DescriptorSetIndex];
			DescriptorSet.Index = DescriptorSetIndex;
			
			const auto& BaseType = Compiler.get_type(SamplerResource.base_type_id);

			uint32_t Binding = Compiler.get_decoration(SamplerResource.id, spv::DecorationBinding);

			if (DescriptorSet.Resources.find(Binding) != DescriptorSet.Resources.end())
			{
				// Binding already exists, just update the stage
				auto& Resource = DescriptorSet.Resources[Binding];
				Resource.Stages |= InStage;
			}
			else
			{
				SamplerShaderResource Resource = {};
				Resource.Stages = InStage;
				Resource.Type = EShaderResourceType::CombinedImageSampler;
				Resource.Name = SamplerResource.name;
				Resource.Binding = Binding;
				Resource.Dimensions = BaseType.image.dim;
				DescriptorSet.Resources[Binding] = Resource;
			}
		}

		for (const auto& PushConstantResource : Resources.push_constant_buffers)
		{
			auto& PushConstant = InCompiledData.PushConstants.emplace_back();
			const auto& BufferType = Compiler.get_type(PushConstantResource.base_type_id);

			PushConstant.Stages = InStage;
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
					auto& OutputParam = InCompiledData.OutputParameters.emplace_back();

					const auto& MemberType = Compiler.get_type(Type.member_types[MemberIdx]);
					OutputParam.Stage = InStage;
					OutputParam.Name = Compiler.get_member_name(Type.self, MemberIdx);
					OutputParam.Location = MemberIdx;
					OutputParam.Format = GetResourceFormat(MemberType);
				}
			}
			else
			{
				auto& OutputParam = InCompiledData.OutputParameters.emplace_back();
				OutputParam.Stage = InStage;
				OutputParam.Name = OutputResource.name;
				OutputParam.Location = Compiler.get_decoration(OutputResource.id, spv::DecorationLocation);
				OutputParam.Format = GetResourceFormat(Type);
			}
		}
	}

	void ShaderCompiler::PrintReflectionData(const CompiledShaderData& InShaderData) const
	{
		LogInfo("Fusion", "Input Parameters:");
		for (const auto& InputParam : InShaderData.InputParameters)
		{
			LogInfo("Fusion", "\tStage: {}", s_ShaderNames.at(InputParam.Stage));
			LogInfo("Fusion", "\tName: {}", InputParam.Name);
			LogInfo("Fusion", "\tLocation: {}", InputParam.Location);
			LogInfo("Fusion", "\t--------------------");
		}

		LogInfo("Fusion", "Push Constants:");
		for (const auto& PushConstant : InShaderData.PushConstants)
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

		LogInfo("Fusion", "Descriptor Sets:");
		for (const auto& DescriptorSet : InShaderData.DescriptorSets)
		{
			LogInfo("Fusion", "\tIndex: {}", DescriptorSet.Index);
			LogInfo("Fusion", "\tResources:");

			for (const auto& Resource : DescriptorSet.Resources | std::views::values)
			{
				LogInfo("Fusion", "\t\tName: {}", Resource.Name);

				LogInfo("Fusion", "\t\tShader Stages:");

				if ((Resource.Stages & EShaderStage::Vertex) != 0) LogInfo("Fusion", "\t\t\t- Vertex");
				if ((Resource.Stages & EShaderStage::Pixel) != 0) LogInfo("Fusion", "\t\t\t- Pixel");

				LogInfo("Fusion", "\t\tType: {}", s_ShaderResourceTypeNames.at(Resource.Type));
				LogInfo("Fusion", "\t\tBinding: {}", Resource.Binding);

				switch (Resource.Type)
				{
				case EShaderResourceType::CombinedImageSampler:
				{
					const auto& SamplerResource = (const SamplerShaderResource&)Resource;
					LogInfo("Fusion", "\t\tDimensions: {}", SamplerResource.Dimensions);
					break;
				}
				}

				LogInfo("Fusion", "\t--------------------");
			}

			LogInfo("Fusion", "\t--------------------");
		}

		LogInfo("Fusion", "Output Parameters:");
		for (const auto& OutputParam : InShaderData.OutputParameters)
		{
			LogInfo("Fusion", "\tStage: {}", s_ShaderNames.at(OutputParam.Stage));
			LogInfo("Fusion", "\tName: {}", OutputParam.Name);
			LogInfo("Fusion", "\tLocation: {}", OutputParam.Location);
			LogInfo("Fusion", "\t--------------------");
		}
	}

}