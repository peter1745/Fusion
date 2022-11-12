#include "FusionPCH.hpp"
#include "D3D11GraphicsPipeline.hpp"
#include "D3D11Context.hpp"
#include "D3D11Shader.hpp"
#include "D3D11CommandList.hpp"

namespace Fusion {

	static constexpr D3D11_FILTER FilterModesToD3D11Filter(EFilterMode MinFilter, EFilterMode MagFilter)
	{
		switch (MinFilter)
		{
		case EFilterMode::Nearest:
		{
			switch (MagFilter)
			{
			case EFilterMode::Nearest: return D3D11_FILTER_MIN_MAG_MIP_POINT;
			case EFilterMode::Linear: return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
			}
			break;
		}
		case EFilterMode::Linear:
		{
			switch (MagFilter)
			{
			case EFilterMode::Nearest: return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
			case EFilterMode::Linear: return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
			}
			break;
		}
		}

		return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	}

	static constexpr D3D11_TEXTURE_ADDRESS_MODE EImageAddressModeToD3D11AddressMode(EImageAddressMode InMode)
	{
		switch (InMode)
		{
		case EImageAddressMode::Wrap: return D3D11_TEXTURE_ADDRESS_WRAP;
		case EImageAddressMode::Mirror: return D3D11_TEXTURE_ADDRESS_MIRROR;
		case EImageAddressMode::Clamp: return D3D11_TEXTURE_ADDRESS_CLAMP;
		case EImageAddressMode::Border: return D3D11_TEXTURE_ADDRESS_BORDER;
		case EImageAddressMode::MirrorOnce: return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
		}

		return D3D11_TEXTURE_ADDRESS_WRAP;
	}

	D3D11GraphicsPipeline::D3D11GraphicsPipeline(const GraphicsPipelineInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo)
	{
		auto Context = GraphicsContext::Get<D3D11Context>();
		auto& Device = Context->GetDevice();

		const auto& PipelineLayoutInfo = InCreateInfo.Layout->GetInfo();

		auto D3DShader = m_CreateInfo.PipelineShader.As<D3D11Shader>();

		D3D11_BLEND_DESC BlendDesc = {};
		BlendDesc.AlphaToCoverageEnable = false;
		BlendDesc.IndependentBlendEnable = false;
		for (uint8_t Idx = 0; Idx < InCreateInfo.RenderTargetCount; Idx++)
		{
			auto& RenderTarget = BlendDesc.RenderTarget[Idx];
			RenderTarget.BlendEnable = InCreateInfo.RenderTargetBlendStates[Idx].EnableBlending;
			//RenderTarget.LogicOpEnable = false;
			RenderTarget.SrcBlend = D3D11_BLEND_SRC_ALPHA;
			RenderTarget.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			RenderTarget.BlendOp = D3D11_BLEND_OP_ADD;
			RenderTarget.SrcBlendAlpha = D3D11_BLEND_ONE;
			RenderTarget.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			RenderTarget.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			//RenderTarget.LogicOp = D3D11_LOGIC_OP_CLEAR;
			RenderTarget.RenderTargetWriteMask = 0xF;
		}
		Device->CreateBlendState(&BlendDesc, m_BlendState);

		D3D11_RASTERIZER_DESC RasterizerDesc = {};
		RasterizerDesc.FillMode = D3D11_FILL_SOLID;
		RasterizerDesc.CullMode = D3D11_CULL_NONE;
		RasterizerDesc.FrontCounterClockwise = InCreateInfo.WindingOrder == EWindingOrder::CounterClockwise;
		RasterizerDesc.DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
		RasterizerDesc.DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
		RasterizerDesc.SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		RasterizerDesc.DepthClipEnable = true;
		RasterizerDesc.MultisampleEnable = false;
		RasterizerDesc.AntialiasedLineEnable = false;
		Device->CreateRasterizerState(&RasterizerDesc, m_RasterizerState);

		D3D11_DEPTH_STENCIL_DESC DepthStencilDesc = {};
		DepthStencilDesc.DepthEnable = IsDepthFormat(InCreateInfo.DepthStencilFormat);
		DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		DepthStencilDesc.StencilEnable = false;
		DepthStencilDesc.StencilReadMask = 0xFF;
		DepthStencilDesc.StencilWriteMask = 0xFF;
		DepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		DepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		DepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		DepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		DepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		DepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		DepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		Device->CreateDepthStencilState(&DepthStencilDesc, m_DepthStencilState);

		std::vector<D3D11_INPUT_ELEMENT_DESC> InputElements(InCreateInfo.Inputs.size());
		for (size_t Idx = 0; Idx < InCreateInfo.Inputs.size(); Idx++)
		{
			const auto& InputData = InCreateInfo.Inputs[Idx];
			auto& InputElement = InputElements[Idx];
			InputElement.SemanticName = InputData.Name.c_str();
			InputElement.SemanticIndex = InputData.Index;
			InputElement.Format = EFormatToDXGIFormat(InputData.Format);
			InputElement.InputSlot = InputData.Binding;
			InputElement.AlignedByteOffset = InputData.Offset;
			InputElement.InputSlotClass = InputData.InstanceStep == 0 ? D3D11_INPUT_PER_VERTEX_DATA : D3D11_INPUT_PER_INSTANCE_DATA;
			InputElement.InstanceDataStepRate = InputData.InstanceStep;
		}

		m_SamplerStates.resize(PipelineLayoutInfo.StaticSamplers.size());
		for (size_t Idx = 0; Idx < PipelineLayoutInfo.StaticSamplers.size(); Idx++)
		{
			const auto& SamplerInfo = PipelineLayoutInfo.StaticSamplers[Idx];

			D3D11_SAMPLER_DESC SamplerDesc = {};
			SamplerDesc.Filter = FilterModesToD3D11Filter(SamplerInfo.MinFilter, SamplerInfo.MagFilter);
			SamplerDesc.AddressU = EImageAddressModeToD3D11AddressMode(SamplerInfo.AddressModeU);
			SamplerDesc.AddressV = EImageAddressModeToD3D11AddressMode(SamplerInfo.AddressModeV);
			SamplerDesc.AddressW = EImageAddressModeToD3D11AddressMode(SamplerInfo.AddressModeW);
			SamplerDesc.MipLODBias = 0;
			SamplerDesc.BorderColor[0] = 0.0f;
			SamplerDesc.BorderColor[1] = 0.0f;
			SamplerDesc.BorderColor[2] = 0.0f;
			SamplerDesc.BorderColor[3] = 1.0f;
			SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			SamplerDesc.MinLOD = 0.0f;
			SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

			Device->CreateSamplerState(&SamplerDesc, m_SamplerStates[Idx]);
		}

		auto VertexByteCode = D3DShader->GetByteCode(EShaderType::Vertex);
		if (VertexByteCode.IsValid())
			Device->CreateInputLayout(InputElements.data(), InputElements.size(), VertexByteCode->GetBufferPointer(), VertexByteCode->GetBufferSize(), m_InputLayout);
	}

	D3D11GraphicsPipeline::~D3D11GraphicsPipeline()
	{

	}

	void D3D11GraphicsPipeline::Bind(CommandList* InCmdList)
	{
		auto& NativeList = static_cast<D3D11CommandList*>(InCmdList)->GetNativeList();
		NativeList->IASetInputLayout(m_InputLayout);
		NativeList->RSSetState(m_RasterizerState);
		NativeList->OMSetBlendState(m_BlendState, NULL, 0xFFFF'FFFF);
		NativeList->OMSetDepthStencilState(m_DepthStencilState, 0);

		auto D3DShader = m_CreateInfo.PipelineShader.As<D3D11Shader>();
		NativeList->VSSetShader(D3DShader->GetVertexShader(), nullptr, 0);
		NativeList->PSSetShader(D3DShader->GetPixelShader(), nullptr, 0);

		// Bind samplers
		{
			const auto& PipelineLayoutInfo = m_CreateInfo.Layout->GetInfo();

			for (size_t Idx = 0; Idx < m_SamplerStates.size(); Idx++)
			{
				const auto& SamplerInfo = PipelineLayoutInfo.StaticSamplers[Idx];

				switch (PipelineLayoutInfo.StaticSamplers[Idx].Visibility)
				{
				case EShaderVisibility::All:
				{
					NativeList->VSSetSamplers(SamplerInfo.Binding, 1, m_SamplerStates[Idx]);
					NativeList->PSSetSamplers(SamplerInfo.Binding, 1, m_SamplerStates[Idx]);
					break;
				}
				case EShaderVisibility::Vertex:
				{
					NativeList->VSSetSamplers(SamplerInfo.Binding, 1, m_SamplerStates[Idx]);
					break;
				}
				case EShaderVisibility::Pixel:
				{
					NativeList->PSSetSamplers(SamplerInfo.Binding, 1, m_SamplerStates[Idx]);
					break;
				}
				}
			}
		}
	}

}
