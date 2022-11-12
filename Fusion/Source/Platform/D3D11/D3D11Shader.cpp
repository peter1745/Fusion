#include "FusionPCH.hpp"
#include "D3D11Shader.hpp"
#include "D3D11Context.hpp"

#include <d3dcompiler.h>

namespace Fusion {

	D3D11Shader::D3D11Shader(const CompiledShaderData& InCompiledData)
		: m_CompiledShaderData(InCompiledData)
	{
		CreateShaders();
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

	void D3D11Shader::CreateShaders()
	{
		ID3D11Device* Device = GraphicsContext::Get<D3D11Context>()->GetDevice();

		if (m_CompiledShaderData.CompiledByteCodes.find(EShaderType::Vertex) != m_CompiledShaderData.CompiledByteCodes.end())
		{
			auto& ByteCode = m_CompiledShaderData.CompiledByteCodes.at(EShaderType::Vertex);

			if (FAILED(Device->CreateVertexShader(ByteCode->GetBufferPointer(), ByteCode->GetBufferSize(), nullptr, m_VertexShader)))
			{
				return;
			}
		}

		if (m_CompiledShaderData.CompiledByteCodes.find(EShaderType::Pixel) != m_CompiledShaderData.CompiledByteCodes.end())
		{
			auto& ByteCode = m_CompiledShaderData.CompiledByteCodes.at(EShaderType::Pixel);

			if (FAILED(Device->CreatePixelShader(ByteCode->GetBufferPointer(), ByteCode->GetBufferSize(), nullptr, m_PixelShader)))
			{
				return;
			}
		}
	}

}
