#include "FusionPCH.hpp"
#include "D3D12GraphicsPipeline.hpp"

#include "D3D12Context.hpp"
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

	static constexpr D3D12_SHADER_VISIBILITY EShaderVisibilityToD3D12ShaderVisibility(EShaderVisibility InVisibility)
	{
		switch (InVisibility)
		{
		case EShaderVisibility::All: return D3D12_SHADER_VISIBILITY_ALL;
		case EShaderVisibility::Vertex: return D3D12_SHADER_VISIBILITY_VERTEX;
		case EShaderVisibility::Pixel: return D3D12_SHADER_VISIBILITY_PIXEL;
		}

		return D3D12_SHADER_VISIBILITY_ALL;
	}

	static constexpr D3D12_ROOT_PARAMETER_TYPE EResourceTypeToRootParameterType(EShaderResourceType InResourceType)
	{
		switch (InResourceType)
		{
		case EShaderResourceType::ConstantBuffer: return D3D12_ROOT_PARAMETER_TYPE_CBV;
		case EShaderResourceType::TextureBuffer: return D3D12_ROOT_PARAMETER_TYPE_SRV;
		case EShaderResourceType::Texture: return D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		case EShaderResourceType::Sampler: return D3D12_ROOT_PARAMETER_TYPE_SRV;
		case EShaderResourceType::UnorderedAccessView: return D3D12_ROOT_PARAMETER_TYPE_UAV;
		case EShaderResourceType::StructuredBuffer: return D3D12_ROOT_PARAMETER_TYPE_SRV;
		case EShaderResourceType::UnorderedAccessViewStructured: return D3D12_ROOT_PARAMETER_TYPE_SRV;
		case EShaderResourceType::ByteAddressBuffer: return D3D12_ROOT_PARAMETER_TYPE_SRV;
		case EShaderResourceType::UAVByteAddressBuffer: return D3D12_ROOT_PARAMETER_TYPE_SRV;
		case EShaderResourceType::UAVAppendStructuredBuffer: return D3D12_ROOT_PARAMETER_TYPE_SRV;
		case EShaderResourceType::UAVConsumeStructuredBuffer: return D3D12_ROOT_PARAMETER_TYPE_SRV;
		case EShaderResourceType::UAVStructuredWithCounter: return D3D12_ROOT_PARAMETER_TYPE_SRV;
		case EShaderResourceType::RTAccelerationStructure: return D3D12_ROOT_PARAMETER_TYPE_SRV;
		case EShaderResourceType::UAVFeedbackTexture: return D3D12_ROOT_PARAMETER_TYPE_SRV;
		}

		return D3D12_ROOT_PARAMETER_TYPE_SRV;
	}

	D3D12GraphicsPipeline::D3D12GraphicsPipeline(const GraphicsPipelineInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo)
	{
		/*auto Context = GraphicsContext::Get<D3D12Context>();
		auto& Device = Context->GetDevice();

		auto D3DShader = m_CreateInfo.PipelineShader.As<D3D12Shader>();

		CreateRootSignature(D3DShader);

		D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineStateDesc = {};
		PipelineStateDesc.pRootSignature = m_PipelineRootSignature;

		auto VertexByteCode = D3DShader->GetByteCode(EShaderType::Vertex);
		if (VertexByteCode.IsValid())
		{
			PipelineStateDesc.VS.pShaderBytecode = VertexByteCode->GetBufferPointer();
			PipelineStateDesc.VS.BytecodeLength = VertexByteCode->GetBufferSize();
		}

		auto PixelByteCode = D3DShader->GetByteCode(EShaderType::Pixel);
		if (PixelByteCode.IsValid())
		{
			PipelineStateDesc.PS.pShaderBytecode = PixelByteCode->GetBufferPointer();
			PipelineStateDesc.PS.BytecodeLength = PixelByteCode->GetBufferSize();
		}

		PipelineStateDesc.BlendState.AlphaToCoverageEnable = false;
		PipelineStateDesc.BlendState.IndependentBlendEnable = false;
		for (uint8_t Idx = 0; Idx < InCreateInfo.RenderTargetCount; Idx++)
		{
			auto& RenderTarget = PipelineStateDesc.BlendState.RenderTarget[Idx];
			RenderTarget.BlendEnable = InCreateInfo.RenderTargetBlendStates[Idx].EnableBlending;
			RenderTarget.LogicOpEnable = false;
			RenderTarget.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			RenderTarget.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			RenderTarget.BlendOp = D3D12_BLEND_OP_ADD;
			RenderTarget.SrcBlendAlpha = D3D12_BLEND_ONE;
			RenderTarget.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
			RenderTarget.BlendOpAlpha = D3D12_BLEND_OP_ADD;
			RenderTarget.LogicOp = D3D12_LOGIC_OP_CLEAR;
			RenderTarget.RenderTargetWriteMask = 0xF;
		}

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
		
		PipelineStateDesc.DepthStencilState.DepthEnable = IsDepthFormat(InCreateInfo.DepthStencilFormat);
		PipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		PipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
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

		const auto& VertexReflectionData = D3DShader->GetReflectedModules().at(EShaderType::Vertex);

		std::vector<D3D12_INPUT_ELEMENT_DESC> InputElements(VertexReflectionData.InputParameters.size());
		for (size_t Idx = 0; Idx < VertexReflectionData.InputParameters.size(); Idx++)
		{
			const auto& InputData = VertexReflectionData.InputParameters[Idx];
			auto& InputElement = InputElements[Idx];

			InputElement.SemanticName = InputData.SemanticName.c_str();
			InputElement.SemanticIndex = InputData.SemanticIndex;
			InputElement.Format = EFormatToDXGIFormat(InputData.Format);
			InputElement.InputSlot = 0;
			InputElement.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
			InputElement.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;// InputData.InstanceStep == 0 ? D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA : D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
			InputElement.InstanceDataStepRate = 0;// InputData.InstanceStep;
		}

		PipelineStateDesc.SampleMask = 0xFFFF'FFFF;

		PipelineStateDesc.InputLayout.pInputElementDescs = InputElements.data();
		PipelineStateDesc.InputLayout.NumElements = InputElements.size();

		PipelineStateDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		
		PipelineStateDesc.PrimitiveTopologyType = EPrimitiveTopologyToD3D12PrimitiveTopology(InCreateInfo.PrimitiveTopology);

		PipelineStateDesc.NumRenderTargets = InCreateInfo.RenderTargetCount;
		for (uint8_t Idx = 0; Idx < InCreateInfo.RenderTargetCount; Idx++)
			PipelineStateDesc.RTVFormats[Idx] = EFormatToDXGIFormat(InCreateInfo.RenderTargetFormats[Idx]);

		PipelineStateDesc.DSVFormat = EFormatToDXGIFormat(InCreateInfo.DepthStencilFormat);

		PipelineStateDesc.SampleDesc.Count = 1;
		PipelineStateDesc.SampleDesc.Quality = 0;

		PipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		HRESULT res = Device->CreateGraphicsPipelineState(&PipelineStateDesc, m_Pipeline, m_Pipeline);*/
	}

	D3D12GraphicsPipeline::~D3D12GraphicsPipeline()
	{

	}

	void D3D12GraphicsPipeline::Bind(CommandList* InCmdList)
	{
		auto& NativeList = static_cast<D3D12CommandList*>(InCmdList)->GetNativeList();
		NativeList->SetPipelineState(m_Pipeline);
		NativeList->SetGraphicsRootSignature(m_PipelineRootSignature);
	}

	void D3D12GraphicsPipeline::CreateRootSignature(const Shared<Shader>& InShader)
	{
		const auto& ReflectedModules = InShader.As<D3D12Shader>()->GetReflectedModules();

		RootSignatureData RootSigData = {};

		for (const auto& [ModuleType, ReflectionData] : ReflectedModules)
			ProcessShaderModule(ModuleType, ReflectionData, RootSigData);

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC RootSignatureDesc = {};
		RootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
		RootSignatureDesc.Desc_1_1.NumParameters = static_cast<UINT>(RootSigData.Parameters.size());
		RootSignatureDesc.Desc_1_1.pParameters = RootSigData.Parameters.data();
		RootSignatureDesc.Desc_1_1.NumStaticSamplers = RootSigData.StaticSamplers.size();
		RootSignatureDesc.Desc_1_1.pStaticSamplers = RootSigData.StaticSamplers.data();
		RootSignatureDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		D3DComPtr<ID3D10Blob> Blob;
		D3DComPtr<ID3D10Blob> Error;
		if (FAILED(D3D12SerializeVersionedRootSignature(&RootSignatureDesc, Blob, Error)))
			return;

		//GraphicsContext::Get<D3D12Context>()->GetDevice()->CreateRootSignature(0, Blob->GetBufferPointer(), Blob->GetBufferSize(), m_PipelineRootSignature, m_PipelineRootSignature);
	}

	void D3D12GraphicsPipeline::ProcessShaderModule(EShaderType InModuleType, const ModuleReflectionData& InReflectionData, RootSignatureData& InData)
	{
		size_t RangeOffset = 0;

		for (const auto& ConstantBuffer : InReflectionData.ConstantBuffers)
		{
			if (InModuleType != EShaderType::Vertex && ConstantBuffer.Visibility == EShaderVisibility::All)
				continue;

			ResourceInfo& Info = m_Resources[ConstantBuffer.Name];
			Info.Name = ConstantBuffer.Name;
			Info.BindingPoint = InData.Parameters.size();
			Info.Visibility = ConstantBuffer.Visibility;

			auto& RootParam = InData.Parameters.emplace_back();
			RootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			RootParam.ShaderVisibility = EShaderVisibilityToD3D12ShaderVisibility(ConstantBuffer.Visibility);

			RootParam.Descriptor.ShaderRegister = ConstantBuffer.BindingPoint;
			RootParam.Descriptor.RegisterSpace = ConstantBuffer.RegisterSpace;
		}

		for (const auto& Resource : InReflectionData.Resources)
		{
			if (InModuleType != EShaderType::Vertex && Resource.Visibility == EShaderVisibility::All)
				continue;

			switch (Resource.Type)
			{
			case EShaderResourceType::Sampler:
			{
				auto& SamplerDesc = InData.StaticSamplers.emplace_back();
				SamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
				SamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
				SamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
				SamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
				SamplerDesc.MipLODBias = 0;
				SamplerDesc.MaxAnisotropy = 0;
				SamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
				SamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
				SamplerDesc.MinLOD = 0.0f;
				SamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
				SamplerDesc.ShaderRegister = Resource.BindingPoint;
				SamplerDesc.RegisterSpace = Resource.RegisterSpace;
				SamplerDesc.ShaderVisibility = EShaderVisibilityToD3D12ShaderVisibility(Resource.Visibility);
				break;
			}
			case EShaderResourceType::Texture:
			{
				ResourceInfo& Info = m_Resources[Resource.Name];
				Info.Name = Resource.Name;
				Info.BindingPoint = InData.Parameters.size();
				Info.Visibility = Resource.Visibility;

				auto& RootParam = InData.Parameters.emplace_back();
				RootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				RootParam.ShaderVisibility = EShaderVisibilityToD3D12ShaderVisibility(Resource.Visibility);

				D3D12_DESCRIPTOR_RANGE1& Range0 = InData.DescriptorRanges.emplace_back();
				Range0.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
				Range0.NumDescriptors = 1;
				Range0.BaseShaderRegister = Resource.BindingPoint;
				Range0.RegisterSpace = Resource.RegisterSpace;
				Range0.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC;
				Range0.OffsetInDescriptorsFromTableStart = 0;

				RootParam.DescriptorTable.NumDescriptorRanges = 1;
				RootParam.DescriptorTable.pDescriptorRanges = &Range0;

				break;
			}
			default:
			{
				ResourceInfo& Info = m_Resources[Resource.Name];
				Info.Name = Resource.Name;
				Info.BindingPoint = InData.Parameters.size();
				Info.Visibility = Resource.Visibility;

				auto& RootParam = InData.Parameters.emplace_back();
				RootParam.ParameterType = EResourceTypeToRootParameterType(Resource.Type);
				RootParam.ShaderVisibility = EShaderVisibilityToD3D12ShaderVisibility(Resource.Visibility);

				RootParam.Descriptor.ShaderRegister = Resource.BindingPoint;
				RootParam.Descriptor.RegisterSpace = Resource.RegisterSpace;
				break;
			}
			}
		}
	}

}
