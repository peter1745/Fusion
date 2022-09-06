#pragma once

#include <filesystem>

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

	public:
		static Shared<Shader> Create(const ShaderSpecification& InSpecification);
	};

}
