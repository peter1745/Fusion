#pragma once

#include "Fusion/Renderer/Shader.hpp"

#include "D3D11Common.hpp"

namespace Fusion {

	class D3D11Shader : public Shader
	{
	public:
		D3D11Shader(const ShaderSpecification& InSpecification);
		~D3D11Shader();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void Set([[maybe_unused]] const std::string& InName, [[maybe_unused]] float InValue) override {}
		virtual void Set([[maybe_unused]] const std::string& InName, [[maybe_unused]] const glm::vec2& InValue) override {}
		virtual void Set([[maybe_unused]] const std::string& InName, [[maybe_unused]] const glm::vec3& InValue) override {}
		virtual void Set([[maybe_unused]] const std::string& InName, [[maybe_unused]] const glm::vec4& InValue) override {}
		virtual void Set([[maybe_unused]] const std::string& InName, [[maybe_unused]] const glm::mat4& InValue, [[maybe_unused]] bool InTranspose = false) override {}
		virtual void Set([[maybe_unused]] const std::string& InName, [[maybe_unused]] const Shared<Texture2D>& InTexture) override {}

		virtual void Set(uint32_t InSlot, const Shared<UniformBuffer>& InBuffer) override;

		auto& GetVertexShader() { return m_VertexShader; }
		const auto& GetVertexShader() const { return m_VertexShader; }

		auto& GetPixelShader() { return m_PixelShader; }
		const auto& GetPixelShader() const { return m_PixelShader; }

		auto& GetVertexByteCode() { return m_VertexByteCode; }
		const auto& GetVertexByteCode() const { return m_VertexByteCode; }

		auto& GetPixelByteCode() { return m_PixelByteCode; }
		const auto& GetPixelByteCode() const { return m_PixelByteCode; }

	private:
		void CompileShader();
		bool CompileFromFile(EShaderType InType, ID3DBlob** OutByteCode);
		void LogCompilerError(const char* InShaderType, ID3DBlob* InErrorMessage) const;

	private:
		ShaderSpecification m_Specification;

		D3DComPtr<ID3DBlob> m_VertexByteCode;
		D3DComPtr<ID3DBlob> m_PixelByteCode;

		D3DComPtr<ID3D11VertexShader> m_VertexShader;
		D3DComPtr<ID3D11PixelShader> m_PixelShader;
		D3DComPtr<ID3D11InputLayout> m_InputLayout;
	};

}
