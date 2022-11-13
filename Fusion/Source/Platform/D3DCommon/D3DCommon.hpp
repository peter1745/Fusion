#pragma once

#include "Fusion/IO/Logging.hpp"
#include "Fusion/Renderer/CommonTypes.hpp"

#include "D3DComPtr.hpp"

#include <dxgi1_6.h>
#include <d3dcommon.h>
#include <dxcapi.h>

namespace Fusion {

	static constexpr DXGI_FORMAT EFormatToDXGIFormat(EFormat InFormat)
	{
		switch (InFormat)
		{
		case EFormat::Unknown: return DXGI_FORMAT_UNKNOWN;
		case EFormat::RGBA32Float: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case EFormat::RGBA32UInt: return DXGI_FORMAT_R32G32B32A32_UINT;
		case EFormat::RGB32Float: return DXGI_FORMAT_R32G32B32_FLOAT;
		case EFormat::RGB32UInt: return DXGI_FORMAT_R32G32B32_UINT;
		case EFormat::RG32Float: return DXGI_FORMAT_R32G32_FLOAT;
		case EFormat::RG32UInt: return DXGI_FORMAT_R32G32_UINT;
		case EFormat::R32Float: return DXGI_FORMAT_R32_FLOAT;
		case EFormat::R32UInt: return DXGI_FORMAT_R32_UINT;
		case EFormat::RGBA8Unorm: return DXGI_FORMAT_R8G8B8A8_UNORM;
		case EFormat::RGBA8UInt: return DXGI_FORMAT_R8G8B8A8_UINT;
		case EFormat::D24UnormS8UInt: return DXGI_FORMAT_D24_UNORM_S8_UINT;
		}

		return DXGI_FORMAT_UNKNOWN;
	}

	static constexpr size_t GetFormatSize(EFormat InFormat)
	{
		switch (InFormat)
		{
		case EFormat::Unknown: return 0;
		case EFormat::RGBA32Float: return 4 * sizeof(float);
		case EFormat::RGBA32UInt: return 4 * sizeof(uint32_t);
		case EFormat::RGB32Float: return 3 * sizeof(float);
		case EFormat::RGB32UInt: return 3 * sizeof(uint32_t);
		case EFormat::RG32Float: return 2 * sizeof(float);
		case EFormat::RG32UInt: return 2 * sizeof(uint32_t);
		case EFormat::R32Float: return 1 * sizeof(float);
		case EFormat::R32UInt: return 1 * sizeof(uint32_t);
		case EFormat::RGBA8Unorm: return 4 * sizeof(uint8_t);
		case EFormat::RGBA8UInt: return 4 * sizeof(uint8_t);
		case EFormat::D24UnormS8UInt:  return 4 * sizeof(uint8_t);
		}

		FUSION_CORE_VERIFY(false);
		return 0;
	}

	// Compiler Structs + Enums

	enum class EShaderType
	{
		Vertex, Pixel
	};

	enum class EShaderResourceType
	{
		ConstantBuffer,
		TextureBuffer,
		Texture,
		Sampler,
		UnorderedAccessView,
		StructuredBuffer,
		UnorderedAccessViewStructured,
		ByteAddressBuffer,
		UAVByteAddressBuffer,
		UAVAppendStructuredBuffer,
		UAVConsumeStructuredBuffer,
		UAVStructuredWithCounter,
		RTAccelerationStructure,
		UAVFeedbackTexture
	};

	struct ShaderSignatureParameter
	{
		std::string SemanticName;
		uint32_t SemanticIndex;
		uint32_t Register;
		EFormat Format;
	};

	struct ShaderBufferVariableInfo
	{
		std::string Name;
		uint32_t Offset;
		uint32_t Size;
	};

	struct ShaderConstantBufferInfo
	{
		std::string Name;
		uint32_t BindingPoint;
		uint32_t BindingCount;
		uint32_t RegisterSpace;
		uint32_t Size;
		std::vector<ShaderBufferVariableInfo> Variables;
		EShaderVisibility Visibility;
	};

	struct ShaderResourceInfo
	{
		std::string Name;
		EShaderResourceType Type;
		uint32_t BindingPoint;
		uint32_t BindingCount;
		uint32_t RegisterSpace;
		EShaderVisibility Visibility;

	};

	struct ModuleReflectionData
	{
		std::vector<ShaderSignatureParameter> InputParameters;
		std::vector<ShaderConstantBufferInfo> ConstantBuffers;
		std::vector<ShaderResourceInfo> Resources;
		std::vector<ShaderSignatureParameter> OutputParameters;
	};

	struct CompiledShaderData
	{
		std::unordered_map<EShaderType, D3DComPtr<ID3DBlob>> CompiledByteCodes;
		std::unordered_map<EShaderType, ModuleReflectionData> ReflectionData;
	};

}
