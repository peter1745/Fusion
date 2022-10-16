#pragma once

#include "Texture.h"
#include "VertexBufferLayout.h"

#include <filesystem>
#include <glm/glm.hpp>

namespace Fusion {

	struct ShaderSpecification
	{
		std::filesystem::path FilePath;

		// TODO(Peter): Rename to ShaderInputLayout
		VertexBufferLayout InputLayout;
	};

	enum class EShaderType
	{
		Vertex, Pixel
	};

	class Shader : public SharedObject
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Set(const std::string& InName, float InValue) = 0;
		virtual void Set(const std::string& InName, const glm::vec2& InValue) = 0;
		virtual void Set(const std::string& InName, const glm::vec3& InValue) = 0;
		virtual void Set(const std::string& InName, const glm::vec4& InValue) = 0;
		virtual void Set(const std::string& InName, const glm::mat4& InValue, bool InTranspose = false) = 0;
		virtual void Set(const std::string& InName, const Shared<Texture2D>& InTexture) = 0;

	public:
		static Shared<Shader> Create(const ShaderSpecification& InSpecification);
	};

}
