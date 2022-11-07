#include "FusionPCH.hpp"
#include "D3D11Shader.hpp"
#include "D3D11Context.hpp"

#include <d3dcompiler.h>

namespace Fusion {

	D3D11Shader::D3D11Shader(const ShaderSpecification& InSpecification)
		: m_Specification(InSpecification)
	{
		CompileShader();
	}

	D3D11Shader::~D3D11Shader()
	{
	}

	void D3D11Shader::Bind()
	{
		ID3D11DeviceContext* Context = GraphicsContext::Get<D3D11Context>()->GetDeviceContext();
		Context->IASetInputLayout(m_InputLayout);
		Context->VSSetShader(m_VertexShader, nullptr, 0);
		Context->PSSetShader(m_PixelShader, nullptr, 0);
	}

	void D3D11Shader::Unbind()
	{
	}

	void D3D11Shader::CompileShader()
	{
		if (!CompileFromFile(EShaderType::Vertex, m_VertexByteCode))
			return;

		if (!CompileFromFile(EShaderType::Pixel, m_PixelByteCode))
		{
			m_VertexByteCode->Release();
			return;
		}

		ID3D11Device* Device = GraphicsContext::Get<D3D11Context>()->GetDevice();
		HRESULT Result = Device->CreateVertexShader(m_VertexByteCode->GetBufferPointer(), m_VertexByteCode->GetBufferSize(), nullptr, m_VertexShader);

		if (FAILED(Result))
		{
			FUSION_CORE_ERROR("Failed to create D3D11 Vertex Shader!");
			m_VertexByteCode->Release();
			m_PixelByteCode->Release();
			return;
		}

		Result = Device->CreatePixelShader(m_PixelByteCode->GetBufferPointer(), m_PixelByteCode->GetBufferSize(), nullptr, m_PixelShader);

		if (FAILED(Result))
		{
			FUSION_CORE_ERROR("Failed to create D3D11 Pixel Shader!");
			m_VertexShader.Release();
			m_VertexByteCode->Release();
			m_PixelByteCode->Release();
			return;
		}
	}

	bool D3D11Shader::CompileFromFile(EShaderType InType, ID3DBlob** OutByteCode)
	{
		const char* EntryPoint = InType == EShaderType::Vertex ? "VertexMain" : "PixelMain";
		const char* TargetVersion = InType == EShaderType::Vertex ? "vs_5_0" : "ps_5_0";

		ID3DBlob* ErrorMessageData;
		HRESULT Result = D3DCompileFromFile(
			m_Specification.FilePath.c_str(),
			nullptr, nullptr,
			EntryPoint, TargetVersion,
			D3DCOMPILE_DEBUG | D3DCOMPILE_ENABLE_STRICTNESS, 0, OutByteCode, &ErrorMessageData);

		if (FAILED(Result))
		{
			LogCompilerError(InType == EShaderType::Vertex ? "vertex" : "pixel", ErrorMessageData);
			*OutByteCode = nullptr;
			return false;
		}

		return true;
	}

	void D3D11Shader::LogCompilerError(const char* InShaderType, ID3DBlob* InErrorMessage) const
	{
		FUSION_CORE_ERROR("Failed to compile {} shader in file {}.", InShaderType, m_Specification.FilePath.string());

		if (InErrorMessage)
		{
			char* ErrorString = reinterpret_cast<char*>(InErrorMessage->GetBufferPointer());
			FUSION_CORE_ERROR("\tError Message: {}", ErrorString);
			InErrorMessage->Release();
		}
		else
		{
			FUSION_CORE_ERROR("\tError Message: Failed to locate shader file!");
		}
	}

}
