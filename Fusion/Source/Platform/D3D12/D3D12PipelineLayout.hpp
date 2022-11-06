#pragma once

#include "D3D12Common.hpp"
#include "Fusion/Renderer/PipelineLayout.hpp"

namespace Fusion {
	
	class D3D12PipelineLayout : public PipelineLayout
	{
	public:
		D3D12PipelineLayout(const PipelineLayoutInfo& InCreateInfo);
		~D3D12PipelineLayout();

		virtual const PipelineLayoutInfo& GetInfo() const override { return m_CreateInfo; }

		auto& GetRootSignature() { return m_RootSignature; }

	private:
		PipelineLayoutInfo m_CreateInfo;
		D3DComPtr<ID3D12RootSignature> m_RootSignature;
	};

}
