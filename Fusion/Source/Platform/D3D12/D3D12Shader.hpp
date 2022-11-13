#pragma once

#include "Fusion/Renderer/Shader.hpp"

#include "D3D12Common.hpp"

namespace Fusion {

	class D3D12Shader : public Shader
	{
	public:
		D3D12Shader(const CompiledShaderData& InCompiledData);
		~D3D12Shader();

		virtual void Bind() override {}
		virtual void Unbind() override {}

		auto GetByteCode(EShaderType InModuleType)
		{
			if (m_CompiledShaderData.CompiledByteCodes.find(InModuleType) == m_CompiledShaderData.CompiledByteCodes.end())
				return D3DComPtr<ID3DBlob>();

			return m_CompiledShaderData.CompiledByteCodes[InModuleType];
		}

		const auto& GetReflectedModules() const { return m_CompiledShaderData.ReflectionData; }

	private:
		CompiledShaderData m_CompiledShaderData;
	};

}
