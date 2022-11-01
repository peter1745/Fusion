#include "FusionPCH.hpp"
#include "D3D12GraphicsPipeline.hpp"

#include "D3D12Context.hpp"
#include "D3D12PipelineLayout.hpp"
#include "D3D12Shader.hpp"

namespace Fusion {

	static D3D12_PRIMITIVE_TOPOLOGY_TYPE EPrimitiveTopologyToD3D12PrimitiveTopology(EPrimitiveTopology InPrimitiveTopology)
	{
		switch (InPrimitiveTopology)
		{
		case EPrimitiveTopology::Triangles: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		}

		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
	}

	D3D12GraphicsPipeline::D3D12GraphicsPipeline(const GraphicsPipelineInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo)
	{
		auto Context = GraphicsContext::Get<D3D12Context>();
		auto& Device = Context->GetDevice();

		D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineStateDesc = {};
		PipelineStateDesc.pRootSignature = static_cast<D3D12PipelineLayout*>(InCreateInfo.Layout)->GetRootSignature();

		auto D3DShader = m_CreateInfo.PipelineShader.As<D3D12Shader>();

		auto& VertexByteCode = D3DShader->GetVertexByteCode();
		if (VertexByteCode.IsValid())
		{
			PipelineStateDesc.VS.pShaderBytecode = VertexByteCode->GetBufferPointer();
			PipelineStateDesc.VS.BytecodeLength = VertexByteCode->GetBufferSize();
		}

		auto& PixelByteCode = D3DShader->GetPixelByteCode();
		if (PixelByteCode.IsValid())
		{
			PipelineStateDesc.PS.pShaderBytecode = PixelByteCode->GetBufferPointer();
			PipelineStateDesc.PS.BytecodeLength = PixelByteCode->GetBufferSize();
		}

		PipelineStateDesc.BlendState.AlphaToCoverageEnable = false;
		PipelineStateDesc.BlendState.IndependentBlendEnable = false;
		//for (std::uint8_t i = 0; i < 8; ++i)
		//{
			auto& renderTarget = PipelineStateDesc.BlendState.RenderTarget[0];
			renderTarget.BlendEnable = true;
			renderTarget.LogicOpEnable = false;
			renderTarget.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			renderTarget.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			renderTarget.BlendOp = D3D12_BLEND_OP_ADD;
			renderTarget.SrcBlendAlpha = D3D12_BLEND_ONE;
			renderTarget.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
			renderTarget.BlendOpAlpha = D3D12_BLEND_OP_ADD;
			renderTarget.LogicOp = D3D12_LOGIC_OP_CLEAR;
			renderTarget.RenderTargetWriteMask = 0xF;
		//}

		PipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		PipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		PipelineStateDesc.RasterizerState.FrontCounterClockwise = InCreateInfo.WindingOrder == EWindingOrder::CounterClockwise;
		PipelineStateDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		PipelineStateDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		PipelineStateDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		PipelineStateDesc.RasterizerState.DepthClipEnable = true;
		PipelineStateDesc.RasterizerState.MultisampleEnable = false;
		PipelineStateDesc.RasterizerState.AntialiasedLineEnable = false;
		PipelineStateDesc.RasterizerState.ForcedSampleCount = 0;
		PipelineStateDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		
		PipelineStateDesc.DepthStencilState.DepthEnable = false;
		PipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		PipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		PipelineStateDesc.DepthStencilState.StencilEnable = false;
		PipelineStateDesc.DepthStencilState.StencilReadMask = 0xFF;
		PipelineStateDesc.DepthStencilState.StencilWriteMask = 0xFF;
		PipelineStateDesc.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		PipelineStateDesc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		PipelineStateDesc.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		PipelineStateDesc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		PipelineStateDesc.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		PipelineStateDesc.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		PipelineStateDesc.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		PipelineStateDesc.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

		std::vector<D3D12_INPUT_ELEMENT_DESC> InputElements(InCreateInfo.Inputs.size());
		for (size_t Idx = 0; Idx < InCreateInfo.Inputs.size(); Idx++)
		{
			const auto& InputData = InCreateInfo.Inputs[Idx];
			auto& InputElement = InputElements[Idx];

			InputElement.SemanticName = InputData.Name.c_str();
			InputElement.SemanticIndex = InputData.Index;
			InputElement.Format = EGraphicsFormatToDXGIFormat(InputData.Format);
			InputElement.InputSlot = InputData.Binding;
			InputElement.AlignedByteOffset = InputData.Offset;
			InputElement.InputSlotClass = InputData.InstanceStep == 0 ? D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA : D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
			InputElement.InstanceDataStepRate = InputData.InstanceStep;
		}

		PipelineStateDesc.SampleMask = 0xFFFF'FFFF;

		PipelineStateDesc.InputLayout.pInputElementDescs = InputElements.data();
		PipelineStateDesc.InputLayout.NumElements = InputElements.size();

		PipelineStateDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		
		PipelineStateDesc.PrimitiveTopologyType = EPrimitiveTopologyToD3D12PrimitiveTopology(InCreateInfo.PrimitiveTopology);

		PipelineStateDesc.NumRenderTargets = InCreateInfo.RenderTargetCount;
		for (uint8_t Idx = 0; Idx < InCreateInfo.RenderTargetCount; Idx++)
			PipelineStateDesc.RTVFormats[Idx] = EGraphicsFormatToDXGIFormat(InCreateInfo.RenderTargetFormats[Idx]);

		PipelineStateDesc.DSVFormat = EGraphicsFormatToDXGIFormat(InCreateInfo.DepthStencilFormat);

		PipelineStateDesc.SampleDesc.Count = 1;
		PipelineStateDesc.SampleDesc.Quality = 0;

		PipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		HRESULT res = Device->CreateGraphicsPipelineState(&PipelineStateDesc, m_Pipeline, m_Pipeline);
	}

	D3D12GraphicsPipeline::~D3D12GraphicsPipeline()
	{

	}

	void D3D12GraphicsPipeline::Bind()
	{
		auto* CmdList = GraphicsContext::Get<D3D12Context>()->GetCurrentCommandList();
		auto& NativeList = static_cast<D3D12CommandList*>(CmdList)->GetNativeList();

		NativeList->SetPipelineState(m_Pipeline);
		NativeList->SetGraphicsRootSignature(static_cast<D3D12PipelineLayout*>(m_CreateInfo.Layout)->GetRootSignature());
	}

}
