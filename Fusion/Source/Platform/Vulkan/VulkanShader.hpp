#pragma once

#include "Fusion/Renderer/Shader.hpp"

#include "VulkanShaderCompiler.hpp"

namespace Fusion {

	class VulkanShader : public Shader
	{
	public:
		explicit VulkanShader(const VulkanCompiledShaderData& InData);

		void Bind() override;
		void Unbind() override;

		auto GetByteCode(EShaderType InModuleType)
		{
			if (m_CompiledData.ModuleByteCodes.find(InModuleType) == m_CompiledData.ModuleByteCodes.end())
				return std::vector<uint32_t>();

			return m_CompiledData.ModuleByteCodes[InModuleType];
		}

		const auto& GetReflectedModules() const { return m_CompiledData.ReflectionData; }

	private:
		VulkanCompiledShaderData m_CompiledData;
	};

}
