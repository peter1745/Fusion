#pragma once

#include "Texture.hpp"

#include <filesystem>
#include <glm/glm.hpp>

namespace Fusion {

	struct ShaderSpecification
	{
		std::filesystem::path FilePath;
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

	public:
		static Shared<Shader> Create(const ShaderSpecification& InSpecification);
	};

}
