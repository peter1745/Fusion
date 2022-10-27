#include "FusionPCH.hpp"
#include "D3D11Shader.hpp"
#include "D3D11Context.hpp"
#include "D3D11UniformBuffer.hpp"

#include <d3dcompiler.h>

namespace Fusion {

	static DXGI_FORMAT ShaderDataTypeToDXGIFormat(ShaderDataType InDataType)
	{
		switch (InDataType)
		{
		case ShaderDataType::Float: return DXGI_FORMAT_R32_FLOAT;
		case ShaderDataType::Float2: return DXGI_FORMAT_R32G32_FLOAT;
		case ShaderDataType::Float3: return DXGI_FORMAT_R32G32B32_FLOAT;
		case ShaderDataType::Float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		FUSION_CORE_VERIFY(false, "Unsupported data type!");
		return DXGI_FORMAT_R32_FLOAT;
	}

	D3D11Shader::D3D11Shader(const ShaderSpecification& InSpecification)
		: m_Specification(InSpecification)
	{
		CompileShader();
	}

	D3D11Shader::~D3D11Shader()
	{
		FUSION_RELEASE_COM(m_InputLayout);
		FUSION_RELEASE_COM(m_PixelShader);
		FUSION_RELEASE_COM(m_VertexShader);
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

	void D3D11Shader::Set(uint32_t InSlot, const Shared<UniformBuffer>& InBuffer)
	{
		if (!InBuffer)
			return;

		Shared<D3D11UniformBuffer> D3DBuffer = InBuffer.As<D3D11UniformBuffer>();

		if (D3DBuffer->GetBuffer() == nullptr)
			return;

		ID3D11DeviceContext* Context = GraphicsContext::Get<D3D11Context>()->GetDeviceContext();
		ID3D11Buffer* BufferPtr = D3DBuffer->GetBuffer();

		switch (D3DBuffer->GetBindPoint())
		{
		case EShaderBindPoint::VertexShader:
			Context->VSSetConstantBuffers(InSlot, 1, &BufferPtr);
			break;
		case EShaderBindPoint::PixelShader:
			Context->PSSetConstantBuffers(InSlot, 1, &BufferPtr);
			break;
		case EShaderBindPoint::Both:
		{
			Context->VSSetConstantBuffers(InSlot, 1, &BufferPtr);
			Context->PSSetConstantBuffers(InSlot, 1, &BufferPtr);
			break;
		}
		}
	}

	void D3D11Shader::CompileShader()
	{
		ID3DBlob* VertexByteCode;
		if (!CompileFromFile(EShaderType::Vertex, &VertexByteCode))
			return;

		ID3DBlob* PixelByteCode;
		if (!CompileFromFile(EShaderType::Pixel, &PixelByteCode))
		{
			FUSION_RELEASE_COM(VertexByteCode);
			return;
		}

		ID3D11Device* Device = GraphicsContext::Get<D3D11Context>()->GetDevice();
		HRESULT Result = Device->CreateVertexShader(VertexByteCode->GetBufferPointer(), VertexByteCode->GetBufferSize(), nullptr, &m_VertexShader);

		if (FAILED(Result))
		{
			FUSION_CORE_ERROR("Failed to create D3D11 Vertex Shader!");
			FUSION_RELEASE_COM(VertexByteCode);
			FUSION_RELEASE_COM(PixelByteCode);
			return;
		}

		Result = Device->CreatePixelShader(PixelByteCode->GetBufferPointer(), PixelByteCode->GetBufferSize(), nullptr, &m_PixelShader);

		if (FAILED(Result))
		{
			FUSION_CORE_ERROR("Failed to create D3D11 Pixel Shader!");
			FUSION_RELEASE_COM(m_VertexShader);
			FUSION_RELEASE_COM(VertexByteCode);
			FUSION_RELEASE_COM(PixelByteCode);
			return;
		}

		CreateInputLayout(VertexByteCode);

		FUSION_RELEASE_COM(VertexByteCode);
		FUSION_RELEASE_COM(PixelByteCode);
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

	void D3D11Shader::CreateInputLayout(ID3DBlob* InVertexByteCode)
	{
		const auto& InputAttributes = m_Specification.InputLayout.GetAttributes();
		std::vector<D3D11_INPUT_ELEMENT_DESC> InputElements(InputAttributes.size());

		for (size_t Idx = 0; Idx < InputAttributes.size(); Idx++)
		{
			const auto& Attrib = InputAttributes[Idx];
			auto& Elem = InputElements[Idx];

			Elem.SemanticName = Attrib.Name;
			Elem.SemanticIndex = 0;
			Elem.Format = ShaderDataTypeToDXGIFormat(Attrib.Type);
			Elem.InputSlot = 0;
			Elem.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			Elem.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			Elem.InstanceDataStepRate = 0;
		}

		ID3D11Device* Device = GraphicsContext::Get<D3D11Context>()->GetDevice();
		Device->CreateInputLayout(
			InputElements.data(), UINT(InputElements.size()),
			InVertexByteCode->GetBufferPointer(), InVertexByteCode->GetBufferSize(),
			&m_InputLayout);
	}

	void D3D11Shader::LogCompilerError(const char* InShaderType, ID3DBlob* InErrorMessage) const
	{
		FUSION_CORE_ERROR("Failed to compile {} shader in file {}.", InShaderType, m_Specification.FilePath.string());

		if (InErrorMessage)
		{
			char* ErrorString = reinterpret_cast<char*>(InErrorMessage->GetBufferPointer());
			FUSION_CORE_ERROR("\tError Message: {}", ErrorString);
			FUSION_RELEASE_COM(InErrorMessage);
		}
		else
		{
			FUSION_CORE_ERROR("\tError Message: Failed to locate shader file!");
		}
	}

}
