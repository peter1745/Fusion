#include "FusionPCH.hpp"
#include "D3D11GraphicsPipeline.hpp"
#include "D3D11Context.hpp"
#include "D3D11Shader.hpp"
#include "D3D11CommandList.hpp"

namespace Fusion {

	D3D11GraphicsPipeline::D3D11GraphicsPipeline(const GraphicsPipelineInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo)
	{
		auto Context = GraphicsContext::Get<D3D11Context>();
		auto& Device = Context->GetDevice();

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

		auto& VertexByteCode = D3DShader->GetVertexByteCode();
		Device->CreateInputLayout(InputElements.data(), InputElements.size(), VertexByteCode->GetBufferPointer(), VertexByteCode->GetBufferSize(), m_InputLayout);
	}

	D3D11GraphicsPipeline::~D3D11GraphicsPipeline()
	{

	}

	void D3D11GraphicsPipeline::Bind(CommandList* InCmdList)
	{
		auto& NativeList = static_cast<D3D11CommandList*>(InCmdList)->GetNativeList();
		NativeList->OMSetBlendState(m_BlendState, NULL, 0xFFFF'FFFF);
		NativeList->RSSetState(m_RasterizerState);
		NativeList->OMSetDepthStencilState(m_DepthStencilState, 0);
		NativeList->IASetInputLayout(m_InputLayout);

		auto D3DShader = m_CreateInfo.PipelineShader.As<D3D11Shader>();
		NativeList->VSSetShader(D3DShader->GetVertexShader(), nullptr, 0);
		NativeList->PSSetShader(D3DShader->GetPixelShader(), nullptr, 0);
	}

}
