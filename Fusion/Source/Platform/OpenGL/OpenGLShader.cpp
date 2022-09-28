#include "FusionPCH.h"
#include "OpenGLShader.h"
#include "OpenGLTexture.h"
#include "OpenGLCommandBuffer.h"

#include "Fusion/Renderer/Renderer.h"
#include "Fusion/IO/FileIO.h"

#include <glad/gl.h>

#include <glm/gtc/type_ptr.hpp>

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

	static GLShaderType ShaderTypeFromString(std::string_view InTypeStr)
	{
		if (InTypeStr.find("vertex") != std::string_view::npos) return GLShaderType::VertexShader;
		if (InTypeStr.find("fragment") != std::string_view::npos) return GLShaderType::FragmentShader;
		if (InTypeStr.find("pixel") != std::string_view::npos) return GLShaderType::FragmentShader;

		FUSION_CORE_VERIFY(false, "Unsupported shader type!");
		return GLShaderType::VertexShader;
	}

	OpenGLShader::OpenGLShader(const ShaderSpecification& InSpecification)
	{
		if (!InSpecification.FilePath.empty())
		{
			// We expect this shader to have both vertex and fragment code in it
			std::array<uint32_t, 2> ShaderIDs = LoadAndCompileShaders(InSpecification.FilePath);
			FUSION_CORE_VERIFY(ShaderIDs[0] != 0 && ShaderIDs[1] != 0);
			CreateProgram(ShaderIDs[0], ShaderIDs[1]);
		}
		else
		{
			uint32_t VertexShaderID = LoadAndCompileShader(InSpecification.VertexFilePath, GLShaderType::VertexShader);
			uint32_t FragmentShaderID = LoadAndCompileShader(InSpecification.FragmentFilePath, GLShaderType::FragmentShader);
			FUSION_CORE_VERIFY(VertexShaderID != 0 && FragmentShaderID != 0);
			CreateProgram(VertexShaderID, FragmentShaderID);
		}
	}

	void OpenGLShader::Set(const std::string& InName, float InValue)
	{
		Renderer::GetActiveCommandBuffer().As<OpenGLCommandBuffer>()->RecordCommand([ShaderID = m_ShaderID, Name = InName, Value = InValue]()
		{
			GLint Location = glGetUniformLocation(ShaderID, Name.c_str());
			glUniform1f(Location, Value);
		});
	}

	void OpenGLShader::Set(const std::string& InName, const glm::vec2& InValue)
	{
		Renderer::GetActiveCommandBuffer().As<OpenGLCommandBuffer>()->RecordCommand([ShaderID = m_ShaderID, Name = InName, Value = InValue]()
		{
			GLint Location = glGetUniformLocation(ShaderID, Name.c_str());
			glUniform2f(Location, Value.x, Value.y);
		});
	}

	void OpenGLShader::Set(const std::string& InName, const glm::vec3& InValue)
	{
		Renderer::GetActiveCommandBuffer().As<OpenGLCommandBuffer>()->RecordCommand([ShaderID = m_ShaderID, Name = InName, Value = InValue]()
		{
			GLint Location = glGetUniformLocation(ShaderID, Name.c_str());
			glUniform3f(Location, Value.x, Value.y, Value.z);
		});
	}

	void OpenGLShader::Set(const std::string& InName, const glm::vec4& InValue)
	{
		Renderer::GetActiveCommandBuffer().As<OpenGLCommandBuffer>()->RecordCommand([ShaderID = m_ShaderID, Name = InName, Value = InValue]()
		{
			GLint Location = glGetUniformLocation(ShaderID, Name.c_str());
			glUniform4f(Location, Value.x, Value.y, Value.z, Value.w);
		});
	}

	void OpenGLShader::Set(const std::string& InName, const glm::mat4& InValue, bool InTranspose)
	{
		Renderer::GetActiveCommandBuffer().As<OpenGLCommandBuffer>()->RecordCommand([ShaderID = m_ShaderID, Name = InName, Value = InValue, Transpose = InTranspose]()
		{
			GLint Location = glGetUniformLocation(ShaderID, Name.c_str());
			glUniformMatrix4fv(Location, 1, Transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(Value));
		});
	}

	void OpenGLShader::Set(const std::string& InName, const Shared<Texture2D>& InTexture)
	{
		Renderer::GetActiveCommandBuffer().As<OpenGLCommandBuffer>()->RecordCommand([ShaderID = m_ShaderID, Name = InName, Value = InTexture]()
		{
			Shared<OpenGLTexture2D> GLTexture = Value.As<OpenGLTexture2D>();
			GLint Location = glGetUniformLocation(ShaderID, Name.c_str());
			glUniform1i(Location, GLTexture->GetTextureSlot());
		});
	}

	void OpenGLShader::CreateProgram(uint32_t InVertexShaderID, uint32_t InFragmentShaderID)
	{
		m_ShaderID = glCreateProgram();
		glAttachShader(m_ShaderID, InVertexShaderID);
		glAttachShader(m_ShaderID, InFragmentShaderID);
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

			glDetachShader(m_ShaderID, InVertexShaderID);
			glDetachShader(m_ShaderID, InFragmentShaderID);

			glDeleteShader(InVertexShaderID);
			glDeleteShader(InFragmentShaderID);

			glDeleteProgram(m_ShaderID);
		}
	}

	std::array<uint32_t, 2> OpenGLShader::LoadAndCompileShaders(const std::filesystem::path& InPath) const
	{
		std::unordered_map<GLShaderType, std::string_view> ShaderSources;

		std::string SourceString;
		if (!FileIO::ReadFileText(InPath, SourceString))
		{
			FUSION_CORE_ERROR("Failed to locate shader file {}", InPath);
			return { 0, 0 };
		}

		std::string_view TypeToken = "#type";
		size_t TypePos = SourceString.find(TypeToken, 0);
		while (TypePos != std::string::npos)
		{
			size_t EOL = SourceString.find_first_of("\r\n", TypePos);
			size_t Begin = TypePos + TypeToken.length() + 1;
			std::string_view TypeStr(SourceString.c_str() + Begin, EOL - Begin);
			GLShaderType ShaderType = ShaderTypeFromString(TypeStr);

			size_t NextLinePos = SourceString.find_first_not_of("\r\n", EOL);
			TypePos = SourceString.find(TypeToken, NextLinePos);
			ShaderSources[ShaderType] = (TypePos == std::string::npos) ? std::string_view(SourceString.c_str() + NextLinePos) : std::string_view(SourceString.c_str() + NextLinePos, TypePos - NextLinePos);
		}

		uint32_t VertexShaderID = CompileShader(ShaderSources[GLShaderType::VertexShader], GLShaderType::VertexShader);
		uint32_t FragmentShaderID = CompileShader(ShaderSources[GLShaderType::FragmentShader], GLShaderType::FragmentShader);
		return { VertexShaderID, FragmentShaderID };
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
			FUSION_CORE_ERROR("Failed to compile shader!");
			FUSION_CORE_ERROR("Error Log: {}", Log);

			glDeleteShader(ShaderID);
			return 0;
		}

		return ShaderID;
	}

	uint32_t OpenGLShader::CompileShader(std::string_view InSource, GLShaderType InType) const
	{
		uint32_t ShaderID = glCreateShader(ShaderTypeToGLType(InType));
		const char* SourceCStr = InSource.data();
		const GLint SourceLength = InSource.length();
		glShaderSource(ShaderID, 1, &SourceCStr, &SourceLength);
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
			FUSION_CORE_ERROR("Failed to compile shader!");
			FUSION_CORE_ERROR("Error Log: {}", Log);

			glDeleteShader(ShaderID);
			return 0;
		}

		return ShaderID;
	}

}
