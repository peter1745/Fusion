#pragma once

#include "Fusion/Renderer/GraphicsPipeline.hpp"

#include "D3D12Common.hpp"

namespace Fusion {

	class D3D12GraphicsPipeline : public GraphicsPipeline
	{
	public:
		D3D12GraphicsPipeline(const GraphicsPipelineInfo& InCreateInfo);
		~D3D12GraphicsPipeline();

		virtual void Bind() override;

		virtual const GraphicsPipelineInfo& GetInfo() const override { return m_CreateInfo; }

	private:
		GraphicsPipelineInfo m_CreateInfo;
		D3D12ComPtr<ID3D12PipelineState> m_Pipeline;
	};

}
