#include "FusionPCH.hpp"
#include "D3D11ShaderCompiler.hpp"
#include "D3D11Shader.hpp"

#include <d3dcompiler.h>

namespace Fusion {

	static std::unordered_map<EShaderType, LPCSTR> s_ShaderEntryPoints =
	{
		{ EShaderType::Vertex, "VertexMain"},
		{ EShaderType::Pixel, "PixelMain"}
	};

	static std::unordered_map<EShaderType, const char*> s_ShaderNames =
	{
		{ EShaderType::Vertex, "vertex"},
		{ EShaderType::Pixel, "pixel"}
	};

	static std::unordered_map<EShaderType, LPCSTR> s_ShaderProfiles =
	{
		{ EShaderType::Vertex, "vs_5_0"},
		{ EShaderType::Pixel, "ps_5_0"}
	};

	static std::unordered_map<D3D_SHADER_INPUT_TYPE, const char*> s_ShaderInputTypeNames = {
		{ D3D_SIT_CBUFFER, "Constant Buffer" },
		{ D3D_SIT_TBUFFER, "Texture Buffer" },
		{ D3D_SIT_TEXTURE, "Texture" },
		{ D3D_SIT_SAMPLER, "Sampler" },
		{ D3D_SIT_UAV_RWTYPED, "Unordered Access View" },
		{ D3D_SIT_STRUCTURED, "Structured Buffers" },
		{ D3D_SIT_UAV_RWSTRUCTURED, "UAV Structured" },
		{ D3D_SIT_BYTEADDRESS, "Byte Address Buffer" },
		{ D3D_SIT_UAV_RWBYTEADDRESS, "UAV Byte Address Buffer" },
		{ D3D_SIT_UAV_APPEND_STRUCTURED, "Append Structured UAV" },
		{ D3D_SIT_UAV_CONSUME_STRUCTURED, "UAV Consume Structured" },
		{ D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER, "UAV Structured with Counter" },
		{ D3D_SIT_RTACCELERATIONSTRUCTURE, "RT Acceleration Structure" },
		{ D3D_SIT_UAV_FEEDBACKTEXTURE, "UAV Feedback Texture" }
	};

	static std::unordered_map<D3D_CBUFFER_TYPE, const char*> s_ConstantBufferTypeNames = {
		{ D3D_CT_CBUFFER, "Constant Buffer" },
		{ D3D_CT_TBUFFER, "Texture Buffer" },
		{ D3D_CT_INTERFACE_POINTERS, "Interface Pointers" },
		{ D3D_CT_RESOURCE_BIND_INFO, "Resource Bind Info" }
	};

	static std::unordered_map<D3D_SHADER_INPUT_TYPE, EShaderResourceType> s_ShaderInputTypeToResourceType = {
		{ D3D_SIT_CBUFFER, EShaderResourceType::ConstantBuffer },
		{ D3D_SIT_TBUFFER, EShaderResourceType::TextureBuffer },
		{ D3D_SIT_TEXTURE, EShaderResourceType::Texture },
		{ D3D_SIT_SAMPLER, EShaderResourceType::Sampler },
		{ D3D_SIT_UAV_RWTYPED, EShaderResourceType::UnorderedAccessView },
		{ D3D_SIT_STRUCTURED, EShaderResourceType::StructuredBuffer },
		{ D3D_SIT_UAV_RWSTRUCTURED,  EShaderResourceType::UnorderedAccessViewStructured },
		{ D3D_SIT_BYTEADDRESS, EShaderResourceType::ByteAddressBuffer },
		{ D3D_SIT_UAV_RWBYTEADDRESS, EShaderResourceType::UAVByteAddressBuffer },
		{ D3D_SIT_UAV_APPEND_STRUCTURED, EShaderResourceType::UAVAppendStructuredBuffer },
		{ D3D_SIT_UAV_CONSUME_STRUCTURED, EShaderResourceType::UAVConsumeStructuredBuffer },
		{ D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER, EShaderResourceType::UAVStructuredWithCounter },
		{ D3D_SIT_RTACCELERATIONSTRUCTURE, EShaderResourceType::RTAccelerationStructure },
		{ D3D_SIT_UAV_FEEDBACKTEXTURE, EShaderResourceType::UAVFeedbackTexture },
	};

	static EFormat GetFormatForSignatureElement(const D3D11_SIGNATURE_PARAMETER_DESC& InElementDesc)
	{
		uint32_t ComponentCount = 0;
		// Why can't we just retrieve the component count...?
		if (InElementDesc.Mask & D3D_COMPONENT_MASK_X) ComponentCount++;
		if (InElementDesc.Mask & D3D_COMPONENT_MASK_Y) ComponentCount++;
		if (InElementDesc.Mask & D3D_COMPONENT_MASK_Z) ComponentCount++;
		if (InElementDesc.Mask & D3D_COMPONENT_MASK_W) ComponentCount++;

		switch (InElementDesc.ComponentType)
		{
		case D3D10_REGISTER_COMPONENT_UNKNOWN: return EFormat::Unknown;
		case D3D10_REGISTER_COMPONENT_UINT32:
		{
			switch (ComponentCount)
			{
			case 1: return EFormat::R32UInt;
			case 2: return EFormat::RG32UInt;
			case 3: return EFormat::RGB32UInt;
			case 4: return EFormat::RGBA32UInt;
			}
			break;
		}
		case D3D10_REGISTER_COMPONENT_SINT32:
		{
			switch (ComponentCount)
			{
			case 1: return EFormat::R32SInt;
			case 2: return EFormat::RG32SInt;
			case 3: return EFormat::RGB32SInt;
			case 4: return EFormat::RGBA32SInt;
			}
			break;
		}
		case D3D10_REGISTER_COMPONENT_FLOAT32:
		{
			switch (ComponentCount)
			{
			case 1: return EFormat::R32Float;
			case 2: return EFormat::RG32Float;
			case 3: return EFormat::RGB32Float;
			case 4: return EFormat::RGBA32Float;
			}
			break;
		}
		}

		return EFormat::Unknown;
	}

	Shared<Shader> D3D11ShaderCompiler::CreateShader(const std::filesystem::path& InFilePath)
	{
		CompiledShaderData CompiledData = {};

		TryCompileAndReflectModule(InFilePath, EShaderType::Vertex, CompiledData);
		TryCompileAndReflectModule(InFilePath, EShaderType::Pixel, CompiledData);

		// Determine shader visibility of all constant buffers in this shader
		std::unordered_map<std::string, std::vector<ShaderConstantBufferInfo*>> ProcessedConstantBuffers;
		for (auto& [ShaderType, ReflectionData] : CompiledData.ReflectionData)
		{
			for (auto& ConstantBuffer : ReflectionData.ConstantBuffers)
			{
				if (ProcessedConstantBuffers.find(ConstantBuffer.Name) != ProcessedConstantBuffers.end())
				{
					ConstantBuffer.Visibility = EShaderVisibility::All;

					for (auto* PrevConstantBuffer : ProcessedConstantBuffers.at(ConstantBuffer.Name))
						PrevConstantBuffer->Visibility = EShaderVisibility::All;
				}
				else
				{
					switch (ShaderType)
					{
					case EShaderType::Vertex:
					{
						ConstantBuffer.Visibility = EShaderVisibility::Vertex;
						break;
					}
					case EShaderType::Pixel:
					{
						ConstantBuffer.Visibility = EShaderVisibility::Pixel;
						break;
					}
					}

					ProcessedConstantBuffers[ConstantBuffer.Name].push_back(&ConstantBuffer);
				}
			}
		}

		// Determine shader visibility of all other resources in this shader
		std::unordered_map<std::string, std::vector<ShaderResourceInfo*>> ProcessedResources;
		for (auto& [ShaderType, ReflectionData] : CompiledData.ReflectionData)
		{
			for (auto& Resource : ReflectionData.Resources)
			{
				if (ProcessedResources.find(Resource.Name) != ProcessedResources.end())
				{
					Resource.Visibility = EShaderVisibility::All;

					for (auto* PrevResource : ProcessedResources.at(Resource.Name))
						PrevResource->Visibility = EShaderVisibility::All;
				}
				else
				{
					switch (ShaderType)
					{
					case EShaderType::Vertex:
					{
						Resource.Visibility = EShaderVisibility::Vertex;
						break;
					}
					case EShaderType::Pixel:
					{
						Resource.Visibility = EShaderVisibility::Pixel;
						break;
					}
					}

					ProcessedResources[Resource.Name].push_back(&Resource);
				}
			}
		}

		return Shared<D3D11Shader>::Create(CompiledData);
	}

	void D3D11ShaderCompiler::TryCompileAndReflectModule(const std::filesystem::path& InFilePath, EShaderType InShaderType, CompiledShaderData& OutData)
	{
		D3DComPtr<ID3DBlob> ByteCode = TryCompileModule(InFilePath, InShaderType);

		if (!ByteCode.IsValid())
			return;

		FUSION_CORE_INFO("Reflecting");
		ReflectShader(ByteCode, OutData.ReflectionData[InShaderType]);

		if (OutData.ReflectionData.find(InShaderType) != OutData.ReflectionData.end())
		{
			const auto& ModuleReflectionInfo = OutData.ReflectionData.at(InShaderType);

			FUSION_CORE_INFO("Input Parameters:");
			for (const auto& InputParam : ModuleReflectionInfo.InputParameters)
			{
				FUSION_CORE_INFO("\tSemantic Name: {}", InputParam.SemanticName);
				FUSION_CORE_INFO("\tSemantic Index: {}", InputParam.SemanticIndex);
				FUSION_CORE_INFO("\tRegister: {}", InputParam.Register);
				FUSION_CORE_INFO("\t--------------------");
			}

			FUSION_CORE_INFO("Constant Buffers:");
			for (const auto& ConstantBufferInfo : ModuleReflectionInfo.ConstantBuffers)
			{
				FUSION_CORE_INFO("\tName: {}", ConstantBufferInfo.Name);
				FUSION_CORE_INFO("\tBinding Point: {}", ConstantBufferInfo.BindingPoint);
				FUSION_CORE_INFO("\tBinding Count: {}", ConstantBufferInfo.BindingCount);
				FUSION_CORE_INFO("\tSize: {}", ConstantBufferInfo.Size);
				FUSION_CORE_INFO("\tVariables:");

				for (const auto& VariableInfo : ConstantBufferInfo.Variables)
				{
					FUSION_CORE_INFO("\t\tName: {}", VariableInfo.Name);
					FUSION_CORE_INFO("\t\tOffset: {}", VariableInfo.Offset);
					FUSION_CORE_INFO("\t\tSize: {}", VariableInfo.Size);
					FUSION_CORE_INFO("\t--------------------");
				}

				FUSION_CORE_INFO("\t--------------------");
			}

			FUSION_CORE_INFO("Resources:");
			for (const auto& ResourceInfo : ModuleReflectionInfo.Resources)
			{
				FUSION_CORE_INFO("\tName: {}", ResourceInfo.Name);
				FUSION_CORE_INFO("\tType: {}", s_ShaderInputTypeNames.at(static_cast<D3D_SHADER_INPUT_TYPE>(ResourceInfo.Type)));
				FUSION_CORE_INFO("\tBinding Point: {}", ResourceInfo.BindingPoint);
				FUSION_CORE_INFO("\tBinding Count: {}", ResourceInfo.BindingCount);
				FUSION_CORE_INFO("\t--------------------");
			}

			FUSION_CORE_INFO("Output Parameters:");
			for (const auto& OutputParam : ModuleReflectionInfo.OutputParameters)
			{
				FUSION_CORE_INFO("\tSemantic Name: {}", OutputParam.SemanticName);
				FUSION_CORE_INFO("\tSemantic Index: {}", OutputParam.SemanticIndex);
				FUSION_CORE_INFO("\tRegister: {}", OutputParam.Register);
				FUSION_CORE_INFO("\t--------------------");
			}
		}

		OutData.CompiledByteCodes[InShaderType] = ByteCode;
	}

	D3DComPtr<ID3DBlob> D3D11ShaderCompiler::TryCompileModule(const std::filesystem::path& InFilePath, EShaderType InShaderType)
	{
		FUSION_CORE_INFO("Compiling {} shader from {} for Direct3D 11", s_ShaderNames.at(InShaderType), InFilePath.string());

		uint32_t Flags = D3DCOMPILE_ALL_RESOURCES_BOUND | D3DCOMPILE_WARNINGS_ARE_ERRORS;

#ifdef FUSION_DEBUG
		Flags |= D3DCOMPILE_DEBUG;
		Flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		Flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

		D3DComPtr<ID3DBlob> ByteCode;
		D3DComPtr<ID3DBlob> Error;
		D3DCompileFromFile(InFilePath.c_str(), nullptr, nullptr, s_ShaderEntryPoints.at(InShaderType), s_ShaderProfiles.at(InShaderType), Flags, 0, ByteCode, Error);
				
		if (Error.IsValid() && Error->GetBufferSize() > 0)
		{
			char* ErrorString = reinterpret_cast<char*>(Error->GetBufferPointer());
			FUSION_CORE_ERROR("Failed! Error: {}", ErrorString);
			Error.Release();
			ByteCode.Release();
			return nullptr;
		}

		FUSION_CORE_INFO("Succeded!");
		return ByteCode;
	}

	void D3D11ShaderCompiler::ReflectShader(D3DComPtr<ID3DBlob> InReflectionData, ModuleReflectionData& OutReflectionData)
	{
		D3DComPtr<ID3D11ShaderReflection> ReflectionInfo;
		D3DReflect(InReflectionData->GetBufferPointer(), InReflectionData->GetBufferSize(), ReflectionInfo, ReflectionInfo);

		D3D11_SHADER_DESC ShaderInfo;
		ReflectionInfo->GetDesc(&ShaderInfo);

		OutReflectionData.InputParameters.resize(ShaderInfo.InputParameters);
		for (uint32_t Idx = 0; Idx < ShaderInfo.InputParameters; Idx++)
		{
			D3D11_SIGNATURE_PARAMETER_DESC SignatureDesc = {};
			ReflectionInfo->GetInputParameterDesc(Idx, &SignatureDesc);

			auto& InputParamInfo = OutReflectionData.InputParameters[Idx];
			InputParamInfo.SemanticName = SignatureDesc.SemanticName;
			InputParamInfo.SemanticIndex = SignatureDesc.SemanticIndex;
			InputParamInfo.Register = SignatureDesc.Register;
			InputParamInfo.Format = GetFormatForSignatureElement(SignatureDesc);
		}

		std::unordered_map<std::string, ShaderConstantBufferInfo*> ConstantBufferInfos;

		OutReflectionData.ConstantBuffers.resize(ShaderInfo.ConstantBuffers);
		for (uint32_t Idx = 0; Idx < ShaderInfo.ConstantBuffers; Idx++)
		{
			ID3D11ShaderReflectionConstantBuffer* ReflectionConstantBuffer = ReflectionInfo->GetConstantBufferByIndex(Idx);
			D3D11_SHADER_BUFFER_DESC ConstantBufferDesc = {};
			ReflectionConstantBuffer->GetDesc(&ConstantBufferDesc);

			FUSION_CORE_VERIFY(ConstantBufferDesc.Type == D3D_CT_CBUFFER);

			auto& ConstantBufferInfo = OutReflectionData.ConstantBuffers[Idx];
			ConstantBufferInfo.Name = ConstantBufferDesc.Name;
			ConstantBufferInfo.BindingCount = UINT32_MAX;
			ConstantBufferInfo.BindingCount = UINT32_MAX;
			ConstantBufferInfo.Size = ConstantBufferDesc.Size;

			ConstantBufferInfo.Variables.resize(ConstantBufferDesc.Variables);
			for (uint32_t VariableIdx = 0; VariableIdx < ConstantBufferDesc.Variables; VariableIdx++)
			{
				ID3D11ShaderReflectionVariable* ReflectionVariable = ReflectionConstantBuffer->GetVariableByIndex(VariableIdx);
				D3D11_SHADER_VARIABLE_DESC VariableDesc = {};
				ReflectionVariable->GetDesc(&VariableDesc);

				auto& ConstantBufferVariableInfo = ConstantBufferInfo.Variables[VariableIdx];
				ConstantBufferVariableInfo.Name = VariableDesc.Name;
				ConstantBufferVariableInfo.Offset = VariableDesc.StartOffset;
				ConstantBufferVariableInfo.Size = VariableDesc.Size;
			}

			ConstantBufferInfos[ConstantBufferInfo.Name] = &ConstantBufferInfo;
		}

		OutReflectionData.Resources.reserve(ShaderInfo.BoundResources);
		for (uint32_t Idx = 0; Idx < ShaderInfo.BoundResources; Idx++)
		{
			D3D11_SHADER_INPUT_BIND_DESC ResourceDesc = {};
			ReflectionInfo->GetResourceBindingDesc(Idx, &ResourceDesc);

			if (ResourceDesc.Type == D3D_SIT_CBUFFER)
			{
				ShaderConstantBufferInfo* ConstantBufferInfo = ConstantBufferInfos[ResourceDesc.Name];
				ConstantBufferInfo->BindingPoint = ResourceDesc.BindPoint;
				ConstantBufferInfo->BindingCount = ResourceDesc.BindCount;
			}
			else
			{
				auto& ResourceInfo = OutReflectionData.Resources.emplace_back();
				ResourceInfo.Name = ResourceDesc.Name;
				ResourceInfo.BindingPoint = ResourceDesc.BindPoint;
				ResourceInfo.BindingCount = ResourceDesc.BindCount;
				ResourceInfo.Type = s_ShaderInputTypeToResourceType.at(ResourceDesc.Type);
			}
		}

		OutReflectionData.OutputParameters.resize(ShaderInfo.OutputParameters);
		for (uint32_t Idx = 0; Idx < ShaderInfo.OutputParameters; Idx++)
		{
			D3D11_SIGNATURE_PARAMETER_DESC SignatureDesc = {};
			ReflectionInfo->GetOutputParameterDesc(Idx, &SignatureDesc);

			auto& OutputParamInfo = OutReflectionData.OutputParameters[Idx];
			OutputParamInfo.SemanticName = SignatureDesc.SemanticName;
			OutputParamInfo.SemanticIndex = SignatureDesc.SemanticIndex;
			OutputParamInfo.Register = SignatureDesc.Register;
			OutputParamInfo.Format = GetFormatForSignatureElement(SignatureDesc);
		}
	}

}
