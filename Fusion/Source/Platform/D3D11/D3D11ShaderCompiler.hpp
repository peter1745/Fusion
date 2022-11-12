#pragma once

#include "Fusion/Renderer/ShaderCompiler.hpp"

#include "Platform/D3DCommon/D3DCommon.hpp"

namespace Fusion {

	class D3D11ShaderCompiler : public ShaderCompiler
	{
	public:
		virtual Shared<Shader> CreateShader(const std::filesystem::path& InFilePath) override;

	private:
		void TryCompileAndReflectModule(const std::filesystem::path& InFilePath, EShaderType InShaderType, CompiledShaderData& OutData);
		D3DComPtr<ID3DBlob> TryCompileModule(const std::filesystem::path& InFilePath, EShaderType InShaderType);
		void ReflectShader(D3DComPtr<ID3DBlob> InReflectionData, ModuleReflectionData& OutReflectionData);
	};

}
