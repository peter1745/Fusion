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

		const auto& VertexReflectionData = D3DShader->GetReflectedModules().at(EShaderType::Vertex);

		std::vector<D3D11_INPUT_ELEMENT_DESC> InputElements(VertexReflectionData.InputParameters.size());
		for (size_t Idx = 0; Idx < VertexReflectionData.InputParameters.size(); Idx++)
		{
			const auto& InputData = VertexReflectionData.InputParameters[Idx];
			auto& InputElement = InputElements[Idx];
			InputElement.SemanticName = InputData.SemanticName.c_str();
			InputElement.SemanticIndex = InputData.SemanticIndex;
			InputElement.Format = EFormatToDXGIFormat(InputData.Format);
			InputElement.InputSlot = 0;
			InputElement.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			InputElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;// InputData.InstanceStep == 0 ? D3D11_INPUT_PER_VERTEX_DATA : D3D11_INPUT_PER_INSTANCE_DATA;
			InputElement.InstanceDataStepRate = 0;// InputData.InstanceStep;
		}

		for (const auto& [ShaderType, ReflectionData] : D3DShader->GetReflectedModules())
		{
			for (const auto& Resource : ReflectionData.ConstantBuffers)
			{
				if (ShaderType != EShaderType::Vertex && Resource.Visibility == EShaderVisibility::All)
					continue;

				ResourceInfo& Info = m_Resources[Resource.Name];
				Info.Name = Resource.Name;
				Info.BindingPoint = Resource.BindingPoint;
				Info.Visibility = Resource.Visibility;
			}

			for (const auto& Resource : ReflectionData.Resources)
			{
				if (ShaderType != EShaderType::Vertex && Resource.Visibility == EShaderVisibility::All)
					continue;

				ResourceInfo& Info = m_Resources[Resource.Name];
				Info.Name = Resource.Name;
				Info.BindingPoint = Resource.BindingPoint;
				Info.Visibility = Resource.Visibility;

				if (Resource.Type != EShaderResourceType::Sampler)
					continue;

				D3D11_SAMPLER_DESC SamplerDesc = {};
				SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;// FilterModesToD3D11Filter(SamplerInfo.MinFilter, SamplerInfo.MagFilter);
				SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;// EImageAddressModeToD3D11AddressMode(SamplerInfo.AddressModeU);
				SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;// EImageAddressModeToD3D11AddressMode(SamplerInfo.AddressModeV);
				SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;// EImageAddressModeToD3D11AddressMode(SamplerInfo.AddressModeW);
				SamplerDesc.MipLODBias = 0;
				SamplerDesc.BorderColor[0] = 0.0f;
				SamplerDesc.BorderColor[1] = 0.0f;
				SamplerDesc.BorderColor[2] = 0.0f;
				SamplerDesc.BorderColor[3] = 1.0f;
				SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
				SamplerDesc.MinLOD = 0.0f;
				SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

				auto& SamplerState = m_SamplerStates.emplace_back();
				SamplerState.Visibility = Resource.Visibility;
				Device->CreateSamplerState(&SamplerDesc, SamplerState.State);
			}
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
			for (auto& SamplerInfo : m_SamplerStates)
			{
				switch (SamplerInfo.Visibility)
				{
				case EShaderVisibility::All:
				{
					NativeList->VSSetSamplers(SamplerInfo.BindingPoint, 1, SamplerInfo.State);
					NativeList->PSSetSamplers(SamplerInfo.BindingPoint, 1, SamplerInfo.State);
					break;
				}
				case EShaderVisibility::Vertex:
				{
					NativeList->VSSetSamplers(SamplerInfo.BindingPoint, 1, SamplerInfo.State);
					break;
				}
				case EShaderVisibility::Pixel:
				{
					NativeList->PSSetSamplers(SamplerInfo.BindingPoint, 1, SamplerInfo.State);
					break;
				}
				}
			}
		}
	}

}
