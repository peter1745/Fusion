#pragma once

#include "Common.hpp"
#include "ShaderCompiler.hpp"

namespace Fusion {

	class Shader : public SharedObject
	{
	public:
		explicit Shader(const Shared<Device>& InDevice, const CompiledShaderData& InData);

		void Bind();
		void Unbind();

		auto GetByteCode(EShaderType InModuleType)
		{
			if (m_CompiledData.ModuleByteCodes.find(InModuleType) == m_CompiledData.ModuleByteCodes.end())
				return std::vector<uint32_t>();

			return m_CompiledData.ModuleByteCodes[InModuleType];
		}

		const auto& GetReflectedModules() const { return m_CompiledData.ReflectionData; }

		const auto& GetShaderModules() const { return m_ShaderModules; }

	private:
		CompiledShaderData m_CompiledData;
		std::unordered_map<EShaderType, VkShaderModule> m_ShaderModules;
	};

}
