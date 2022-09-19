#pragma once

#include "Texture.h"

#include <filesystem>
#include <glm/glm.hpp>

namespace Fusion {

	struct ShaderSpecification
	{
		std::filesystem::path VertexFilePath;
		std::filesystem::path FragmentFilePath;
	};

	class Shader
	{
	public:
		virtual ~Shader() = default;

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
