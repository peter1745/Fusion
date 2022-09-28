#pragma once

#include "Fusion/Renderer/Shader.h"

namespace Fusion {

	enum class GLShaderType { VertexShader, FragmentShader };

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const ShaderSpecification& InSpecification);

		uint32_t GetShaderID() const { return m_ShaderID; }

		virtual void Set(const std::string& InName, float InValue) override;
		virtual void Set(const std::string& InName, const glm::vec2& InValue) override;
		virtual void Set(const std::string& InName, const glm::vec3& InValue) override;
		virtual void Set(const std::string& InName, const glm::vec4& InValue) override;
		virtual void Set(const std::string& InName, const glm::mat4& InValue, bool InTranspose = false) override;
		virtual void Set(const std::string& InName, const Shared<Texture2D>& InTexture) override;

	private:
		void CreateProgram(uint32_t InVertexShaderID, uint32_t InFragmentShaderID);
		std::array<uint32_t, 2> LoadAndCompileShaders(const std::filesystem::path& InPath) const;
		uint32_t LoadAndCompileShader(const std::filesystem::path& InPath, GLShaderType InType) const;
		uint32_t CompileShader(std::string_view InSource, GLShaderType InType) const;
	private:
		uint32_t m_ShaderID = 0;
	};

}
