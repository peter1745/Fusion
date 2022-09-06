#include "FusionPCH.h"
#include "OpenGLShader.h"

#include "Fusion/IO/FileIO.h"

#include <glad/gl.h>

namespace Fusion {

	static GLenum ShaderTypeToGLType(GLShaderType InType)
	{
		switch (InType)
		{
		case Fusion::GLShaderType::VertexShader: return GL_VERTEX_SHADER;
		case Fusion::GLShaderType::FragmentShader: return GL_FRAGMENT_SHADER;
		}

		return GL_NONE;
	}

	OpenGLShader::OpenGLShader(const ShaderSpecification& InSpecification)
	{
		uint32_t VertexShaderID = LoadAndCompileShader(InSpecification.VertexFilePath, GLShaderType::VertexShader);
		uint32_t FragmentShaderID = LoadAndCompileShader(InSpecification.FragmentFilePath, GLShaderType::FragmentShader);
		FUSION_CORE_VERIFY(VertexShaderID != 0 && FragmentShaderID != 0);
		
		m_ShaderID = glCreateProgram();
		glAttachShader(m_ShaderID, VertexShaderID);
		glAttachShader(m_ShaderID, FragmentShaderID);
		glLinkProgram(m_ShaderID);

		GLint LinkStatus;
		glGetProgramiv(m_ShaderID, GL_LINK_STATUS, &LinkStatus);
		if (LinkStatus == GL_FALSE)
		{
			GLint LogLength;
			glGetProgramiv(m_ShaderID, GL_INFO_LOG_LENGTH, &LogLength);
			std::string Log;
			Log.reserve(LogLength);

			glGetProgramInfoLog(m_ShaderID, LogLength, NULL, Log.data());
			FUSION_CORE_ERROR("Failed to link shader program!");
			FUSION_CORE_ERROR("Error Log: {}", Log);

			glDetachShader(m_ShaderID, VertexShaderID);
			glDetachShader(m_ShaderID, FragmentShaderID);

			glDeleteShader(VertexShaderID);
			glDeleteShader(FragmentShaderID);

			glDeleteProgram(m_ShaderID);
		}
	}

	uint32_t OpenGLShader::LoadAndCompileShader(const std::filesystem::path& InPath, GLShaderType InType) const
	{
		std::string SourceString;
		if (!FileIO::ReadFileText(InPath, SourceString))
		{
			FUSION_CORE_ERROR("Failed to locate shader file {}", InPath);
			return 0;
		}

		uint32_t ShaderID = glCreateShader(ShaderTypeToGLType(InType));
		const char* Source = SourceString.c_str();
		glShaderSource(ShaderID, 1, &Source, NULL);
		glCompileShader(ShaderID);

		GLint CompileStatus;
		glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &CompileStatus);
		if (CompileStatus == GL_FALSE)
		{
			GLint LogLength;
			glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &LogLength);
			std::string Log;
			Log.reserve(LogLength);

			glGetShaderInfoLog(ShaderID, LogLength, NULL, Log.data());
			FUSION_CORE_ERROR("Failed to compile shader {0}!");
			FUSION_CORE_ERROR("Error Log: {}", Log);

			glDeleteShader(ShaderID);
			return 0;
		}

		return ShaderID;
	}

}
