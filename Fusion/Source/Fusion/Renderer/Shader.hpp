#pragma once

#include "Texture.hpp"

#include <filesystem>
#include <glm/glm.hpp>

namespace Fusion {

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
	};

}
