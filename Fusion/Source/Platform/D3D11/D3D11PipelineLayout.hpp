#pragma once

#include "Fusion/Renderer/PipelineLayout.hpp"

namespace Fusion {

	class D3D11PipelineLayout : public PipelineLayout
	{
	public:
		D3D11PipelineLayout(const PipelineLayoutInfo& InCreateInfo);

		virtual const PipelineLayoutInfo& GetInfo() const override { return m_CreateInfo; }

	private:
		PipelineLayoutInfo m_CreateInfo;
	};

}
