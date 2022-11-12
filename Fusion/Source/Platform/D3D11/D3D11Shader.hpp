#pragma once

#include "Fusion/Renderer/Shader.hpp"

#include "D3D11Common.hpp"

namespace Fusion {

	class D3D11Shader : public Shader
	{
	public:
		D3D11Shader(const CompiledShaderData& InCompiledData);
		~D3D11Shader();

		virtual void Bind() override;
		virtual void Unbind() override;

		auto& GetVertexShader() { return m_VertexShader; }
		const auto& GetVertexShader() const { return m_VertexShader; }

		auto& GetPixelShader() { return m_PixelShader; }
		const auto& GetPixelShader() const { return m_PixelShader; }

		auto GetByteCode(EShaderType InModuleType)
		{
			if (m_CompiledShaderData.CompiledByteCodes.find(InModuleType) == m_CompiledShaderData.CompiledByteCodes.end())
				return D3DComPtr<ID3DBlob>();

			return m_CompiledShaderData.CompiledByteCodes[InModuleType];
		}


	private:
		void CreateShaders();

	private:
		CompiledShaderData m_CompiledShaderData;

		D3DComPtr<ID3D11VertexShader> m_VertexShader;
		D3DComPtr<ID3D11PixelShader> m_PixelShader;
		D3DComPtr<ID3D11InputLayout> m_InputLayout;
	};

}
