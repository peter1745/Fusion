#include "FusionPCH.hpp"
#include "D3D12Shader.hpp"
#include "D3D12Context.hpp"

#include <d3dcompiler.h>

namespace Fusion {

	D3D12Shader::D3D12Shader(const ShaderSpecification& InCreateInfo)
		: m_CreateInfo(InCreateInfo)
	{
		FUSION_CORE_VERIFY(CompileFromFile(EShaderType::Vertex, m_VertexByteCode));
		FUSION_CORE_VERIFY(CompileFromFile(EShaderType::Pixel, m_PixelByteCode));
	}

	D3D12Shader::~D3D12Shader()
	{
	}

	bool D3D12Shader::CompileFromFile(EShaderType InType, D3DComPtr<ID3DBlob>& OutByteCode) const
	{
		const char* EntryPoint = InType == EShaderType::Vertex ? "VertexMain" : "PixelMain";
		const char* TargetVersion = InType == EShaderType::Vertex ? "vs_5_1" : "ps_5_1";

		D3DComPtr<ID3DBlob> ErrorMessageData;
		HRESULT Result = D3DCompileFromFile(
			m_CreateInfo.FilePath.c_str(),
			nullptr, nullptr,
			EntryPoint, TargetVersion,
			D3DCOMPILE_DEBUG | D3DCOMPILE_ENABLE_STRICTNESS, 0, OutByteCode, ErrorMessageData);

		if (FAILED(Result))
		{
			LogCompilerError(InType == EShaderType::Vertex ? "vertex" : "pixel", ErrorMessageData);
			OutByteCode.Release();
			return false;
		}

		return true;
	}

	void D3D12Shader::LogCompilerError(const char* InShaderType, D3DComPtr<ID3DBlob> InErrorMessage) const
	{
		FUSION_CORE_ERROR("Failed to compile {} shader in file {}.", InShaderType, m_CreateInfo.FilePath.string());

		if (InErrorMessage.IsValid())
		{
			char* ErrorString = reinterpret_cast<char*>(InErrorMessage->GetBufferPointer());
			FUSION_CORE_ERROR("\tError Message: {}", ErrorString);
		}
		else
		{
			FUSION_CORE_ERROR("\tError Message: Failed to locate shader file!");
		}
	}

}
