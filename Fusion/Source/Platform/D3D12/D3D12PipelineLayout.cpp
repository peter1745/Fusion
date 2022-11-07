#include "FusionPCH.hpp"
#include "D3D12PipelineLayout.hpp"

#include "D3D12Context.hpp"

namespace Fusion {

	static constexpr D3D12_ROOT_PARAMETER_TYPE EPipelineParameterTypeToD3D12ParameterType(EPipelineParameterType InType)
	{
		switch (InType)
		{
		case EPipelineParameterType::DescriptorTable: return D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		case EPipelineParameterType::Constants: return D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		case EPipelineParameterType::ConstantBufferView: return D3D12_ROOT_PARAMETER_TYPE_CBV;
		case EPipelineParameterType::ShaderResourceView: return D3D12_ROOT_PARAMETER_TYPE_SRV;
		case EPipelineParameterType::UnorderedAccessView: return D3D12_ROOT_PARAMETER_TYPE_UAV;
		}

		return static_cast<D3D12_ROOT_PARAMETER_TYPE>(-1); // UB
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

	static constexpr D3D12_DESCRIPTOR_RANGE_TYPE EDescriptorRangeTypeToD3D12DescriptorRangeType(EPipelineLayoutDescriptorRangeType InType)
	{
		switch (InType)
		{
		case EPipelineLayoutDescriptorRangeType::ConstantBufferView: return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		case EPipelineLayoutDescriptorRangeType::ShaderResourceView: return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		case EPipelineLayoutDescriptorRangeType::UnorderedAccessview: return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		case EPipelineLayoutDescriptorRangeType::Sampler: return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
		}

		return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
	}

	static constexpr D3D12_DESCRIPTOR_RANGE_FLAGS EDescriptorRangeFlagsD3D12DescriptorRangeFlags(EPipelineLayoutDescriptorRangeFlags InFlags)
	{
		uint32_t Result = 0;

		if (InFlags & DescriptorRangeFlags::VolatileDescriptor) Result |= D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE;
		if (InFlags & DescriptorRangeFlags::VolatileData) Result |= D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE;
		if (InFlags & DescriptorRangeFlags::StaticData) Result |= D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC;

		return static_cast<D3D12_DESCRIPTOR_RANGE_FLAGS>(Result);
	}

	static constexpr D3D12_ROOT_SIGNATURE_FLAGS EPipelineLayoutFlagsToD3D12RootSignatureFlags(EPipelineLayoutFlags InFlags)
	{
		uint32_t Result = 0;

		if (InFlags & PipelineLayoutFlags::AllowInputAssemblerInputLayout) Result |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		return static_cast<D3D12_ROOT_SIGNATURE_FLAGS>(Result);
	}

	static constexpr D3D12_FILTER FilterModesToD3D12Filter(EFilterMode MinFilter, EFilterMode MagFilter)
	{
		switch (MinFilter)
		{
		case EFilterMode::Nearest:
		{
			switch (MagFilter)
			{
			case EFilterMode::Nearest: return D3D12_FILTER_MIN_MAG_MIP_POINT;
			case EFilterMode::Linear: return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
			}
			break;
		}
		case EFilterMode::Linear:
		{
			switch (MagFilter)
			{
			case EFilterMode::Nearest: return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
			case EFilterMode::Linear: return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
			}
			break;
		}
		}

		return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	}

	static constexpr D3D12_TEXTURE_ADDRESS_MODE EImageAddressModeToD3D12AddressMode(EImageAddressMode InMode)
	{
		switch (InMode)
		{
		case EImageAddressMode::Wrap: return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		case EImageAddressMode::Mirror: return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
		case EImageAddressMode::Clamp: return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		case EImageAddressMode::Border: return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		case EImageAddressMode::MirrorOnce: return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
		}

		return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	}

	D3D12PipelineLayout::D3D12PipelineLayout(const PipelineLayoutInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo)
	{
		auto Context = GraphicsContext::Get<D3D12Context>();
		auto& Device = Context->GetDevice();

		D3D12_FEATURE_DATA_ROOT_SIGNATURE FeatureData = {};
		FeatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
		if (FAILED(Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &FeatureData, sizeof(FeatureData))))
			return;

		size_t TotalRangeCount = 0;
		for (size_t Idx = 0; Idx < InCreateInfo.Parameters.size(); Idx++)
		{
			const auto& Param = InCreateInfo.Parameters[Idx];
			if (Param.Type == EPipelineParameterType::DescriptorTable)
			{
				TotalRangeCount += std::get<PipelineLayoutDescriptorTable>(Param.Value).Ranges.size();
			}
		}

		std::vector<D3D12_DESCRIPTOR_RANGE1> DescriptorRanges(TotalRangeCount);
		size_t RangeOffset = 0;
		std::vector<D3D12_ROOT_PARAMETER1> Parameters(InCreateInfo.Parameters.size());

		for (size_t Idx = 0; Idx < Parameters.size(); Idx++)
		{
			const auto& Param = InCreateInfo.Parameters[Idx];
			auto& RootParam = Parameters[Idx];
			RootParam.ParameterType = EPipelineParameterTypeToD3D12ParameterType(Param.Type);
			RootParam.ShaderVisibility = EShaderVisibilityToD3D12ShaderVisibility(Param.Visibility);

			switch (Param.Type)
			{
			case EPipelineParameterType::DescriptorTable:
			{
				const auto& DescriptorTable = std::get<PipelineLayoutDescriptorTable>(Param.Value);
				D3D12_DESCRIPTOR_RANGE1* Ranges = DescriptorRanges.data() + RangeOffset;
				RangeOffset += DescriptorTable.Ranges.size();

				RootParam.DescriptorTable.NumDescriptorRanges = static_cast<UINT>(DescriptorTable.Ranges.size());
				RootParam.DescriptorTable.pDescriptorRanges = Ranges;

				for (size_t RangeIdx = 0; RangeIdx < DescriptorTable.Ranges.size(); RangeIdx++)
				{
					const auto& TableRange = DescriptorTable.Ranges[RangeIdx];
					auto& Range = Ranges[RangeIdx];
					Range.RangeType = EDescriptorRangeTypeToD3D12DescriptorRangeType(TableRange.Type);
					Range.NumDescriptors = TableRange.NumDescriptors;
					Range.BaseShaderRegister = TableRange.Binding;
					Range.RegisterSpace = TableRange.Space;
					Range.Flags = EDescriptorRangeFlagsD3D12DescriptorRangeFlags(TableRange.Flags);
					Range.OffsetInDescriptorsFromTableStart = TableRange.Offset;
				}

				break;
			}
			case EPipelineParameterType::Constants:
			{
				const auto& Constants = std::get<PipelineLayoutConstants>(Param.Value);
				RootParam.Constants.ShaderRegister = Constants.Binding;
				RootParam.Constants.RegisterSpace = Constants.Space;
				RootParam.Constants.Num32BitValues = Constants.Num32BitValues;
				break;
			}
			case EPipelineParameterType::ConstantBufferView:
			case EPipelineParameterType::ShaderResourceView:
			case EPipelineParameterType::UnorderedAccessView:
			{
				const auto& Descriptor = std::get<PipelineLayoutDescriptor>(Param.Value);
				RootParam.Descriptor.ShaderRegister = Descriptor.Binding;
				RootParam.Descriptor.RegisterSpace = Descriptor.Space;
				break;
			}
			}
		}

		std::vector<D3D12_STATIC_SAMPLER_DESC> StaticSamplers(InCreateInfo.StaticSamplers.size());
		for (size_t Idx = 0; Idx < InCreateInfo.StaticSamplers.size(); Idx++)
		{
			const auto& SamplerInfo = InCreateInfo.StaticSamplers[Idx];
			auto& SamplerDesc = StaticSamplers[Idx];
			SamplerDesc.Filter = FilterModesToD3D12Filter(SamplerInfo.MinFilter, SamplerInfo.MagFilter);
			SamplerDesc.AddressU = EImageAddressModeToD3D12AddressMode(SamplerInfo.AddressModeU);
			SamplerDesc.AddressV = EImageAddressModeToD3D12AddressMode(SamplerInfo.AddressModeV);
			SamplerDesc.AddressW = EImageAddressModeToD3D12AddressMode(SamplerInfo.AddressModeW);
			SamplerDesc.MipLODBias = 0;
			SamplerDesc.MaxAnisotropy = 0;
			SamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
			SamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
			SamplerDesc.MinLOD = 0.0f;
			SamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
			SamplerDesc.ShaderRegister = SamplerInfo.Binding;
			SamplerDesc.RegisterSpace = SamplerInfo.Register;
			SamplerDesc.ShaderVisibility = EShaderVisibilityToD3D12ShaderVisibility(SamplerInfo.Visibility);
		}

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC RootSignatureDesc = {};
		RootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
		RootSignatureDesc.Desc_1_1.NumParameters = static_cast<UINT>(Parameters.size());
		RootSignatureDesc.Desc_1_1.pParameters = Parameters.data();
		RootSignatureDesc.Desc_1_1.NumStaticSamplers = StaticSamplers.size();
		RootSignatureDesc.Desc_1_1.pStaticSamplers = StaticSamplers.data();
		RootSignatureDesc.Desc_1_1.Flags = EPipelineLayoutFlagsToD3D12RootSignatureFlags(InCreateInfo.Flags);

		D3DComPtr<ID3D10Blob> Blob;
		D3DComPtr<ID3D10Blob> Error;
		if (FAILED(D3D12SerializeVersionedRootSignature(&RootSignatureDesc, Blob, Error)))
			return;

		HRESULT hr = Device->CreateRootSignature(0, Blob->GetBufferPointer(), Blob->GetBufferSize(), m_RootSignature, m_RootSignature);
	}

	D3D12PipelineLayout::~D3D12PipelineLayout()
	{

	}

}
