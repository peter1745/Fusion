#pragma once

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

		virtual const ShaderSpecification& GetSpecification() const = 0;
	};

}
