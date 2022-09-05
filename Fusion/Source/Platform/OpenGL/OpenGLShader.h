#pragma once

#include "Fusion/Renderer/Shader.h"

namespace Fusion {

	enum class GLShaderType { VertexShader, FragmentShader };

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const ShaderSpecification& InSpecification);

		virtual void Bind() const override;

	private:
		uint32_t LoadAndCompileShader(const std::filesystem::path& InPath, GLShaderType InType) const;

	private:
		uint32_t m_ShaderID = 0;
	};

}
