#pragma once

#include "Fusion/Renderer/Shader.hpp"

#include "VulkanCommon.hpp"
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

		const auto& GetShaderModules() const { return m_ShaderModules; }

	private:
		VulkanCompiledShaderData m_CompiledData;
		std::unordered_map<EShaderType, VkShaderModule> m_ShaderModules;
	};

}
