#pragma once

#include "Fusion/Renderer/Shader.hpp"

#include "D3D12Common.hpp"

namespace Fusion {

	class D3D12Shader : public Shader
	{
	public:
		D3D12Shader(const ShaderSpecification& InCreateInfo);
		~D3D12Shader();

		virtual void Bind() override {}
		virtual void Unbind() override {}

		auto& GetVertexByteCode() { return m_VertexByteCode; }
		auto& GetPixelByteCode() { return m_PixelByteCode; }

	private:
		bool CompileFromFile(EShaderType InType, D3DComPtr<ID3DBlob>& OutByteCode) const;
		void LogCompilerError(const char* InShaderType, D3DComPtr<ID3DBlob> InErrorMessage) const;

	private:
		ShaderSpecification m_CreateInfo;

		D3DComPtr<ID3DBlob> m_VertexByteCode;
		D3DComPtr<ID3DBlob> m_PixelByteCode;
	};

}
