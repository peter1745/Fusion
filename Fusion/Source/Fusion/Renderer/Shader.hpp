#pragma once

#include "Texture.hpp"

#include <filesystem>
#include <glm/glm.hpp>

namespace Fusion {

	class Shader : public SharedObject
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;
	};

}
