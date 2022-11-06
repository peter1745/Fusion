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

		virtual void Set(const std::string& InName, float InValue) override {}
		virtual void Set(const std::string& InName, const glm::vec2& InValue) override {}
		virtual void Set(const std::string& InName, const glm::vec3& InValue) override {}
		virtual void Set(const std::string& InName, const glm::vec4& InValue) override {}
		virtual void Set(const std::string& InName, const glm::mat4& InValue, bool InTranspose = false) override {}
		virtual void Set(const std::string& InName, const Shared<Texture2D>& InTexture) override {}

		virtual void Set(uint32_t InSlot, const Shared<UniformBuffer>& InBuffer) override {}

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
