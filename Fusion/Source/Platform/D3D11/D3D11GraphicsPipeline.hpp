#pragma once

#include "Fusion/Renderer/GraphicsPipeline.hpp"

#include "D3D11Common.hpp"

namespace Fusion {

	class D3D11GraphicsPipeline : public GraphicsPipeline
	{
	public:
		D3D11GraphicsPipeline(const GraphicsPipelineInfo& InCreateInfo);
		~D3D11GraphicsPipeline();

		virtual void Bind(CommandList* InCmdList) override;

		virtual const GraphicsPipelineInfo& GetInfo() const override { return m_CreateInfo; }

	private:
		GraphicsPipelineInfo m_CreateInfo;
		D3DComPtr<ID3D11BlendState> m_BlendState;
		D3DComPtr<ID3D11RasterizerState> m_RasterizerState;
		D3DComPtr<ID3D11DepthStencilState> m_DepthStencilState;
		D3DComPtr<ID3D11InputLayout> m_InputLayout;

		std::vector<D3DComPtr<ID3D11SamplerState>> m_SamplerStates;
	};

}
