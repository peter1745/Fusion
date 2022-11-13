#pragma once

#include "Fusion/Renderer/GraphicsPipeline.hpp"

#include "D3D12Common.hpp"

namespace Fusion {

	class D3D12GraphicsPipeline : public GraphicsPipeline
	{
	public:
		D3D12GraphicsPipeline(const GraphicsPipelineInfo& InCreateInfo);
		~D3D12GraphicsPipeline();

		virtual void Bind(CommandList* InCmdLists) override;

		virtual const ResourceInfo& GetResourceInfo(const std::string& InName) const override
		{
			return m_Resources.at(InName);
		}

		virtual const GraphicsPipelineInfo& GetInfo() const override { return m_CreateInfo; }

	private:
		struct RootSignatureData
		{
			std::vector<D3D12_ROOT_PARAMETER1> Parameters;
			std::vector<D3D12_STATIC_SAMPLER_DESC> StaticSamplers;
			std::vector<D3D12_DESCRIPTOR_RANGE1> DescriptorRanges;
		};

		void CreateRootSignature(const Shared<Shader>& InShader);
		void ProcessShaderModule(EShaderType InModuleType, const ModuleReflectionData& InReflectionData, RootSignatureData& InData);

	private:
		GraphicsPipelineInfo m_CreateInfo;
		D3DComPtr<ID3D12RootSignature> m_PipelineRootSignature;
		D3DComPtr<ID3D12PipelineState> m_Pipeline;

		std::unordered_map<std::string, ResourceInfo> m_Resources;
	};

}
