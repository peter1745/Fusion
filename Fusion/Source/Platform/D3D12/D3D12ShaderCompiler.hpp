#pragma once

#include "Fusion/Renderer/ShaderCompiler.hpp"
#include "Platform/D3DCommon/D3DCommon.hpp"

#include <dxcapi.h>

namespace Fusion {

	class D3D12ShaderCompiler : public ShaderCompiler
	{
	public:
		D3D12ShaderCompiler();
		~D3D12ShaderCompiler();

		virtual Shared<Shader> CreateShader(const std::filesystem::path& InFilePath) override;

	private:
		void TryCompileAndReflectModule(const std::filesystem::path& InFilePath, EShaderType InShaderType, CompiledShaderData& OutData);
		D3DComPtr<IDxcResult> TryCompileModule(const std::filesystem::path& InFilePath, EShaderType InShaderType);

		void ReflectShader(D3DComPtr<IDxcBlob> InReflectionData, ModuleReflectionData& OutReflectionData);
	};

}
