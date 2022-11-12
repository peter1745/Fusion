#pragma once

#include "Shader.hpp"

namespace Fusion {

	class ShaderCompiler
	{
	public:
		virtual ~ShaderCompiler() = default;
		virtual Shared<Shader> CreateShader(const std::filesystem::path& InFilePath) = 0;

	public:
		static Unique<ShaderCompiler> Create();
	};

}
