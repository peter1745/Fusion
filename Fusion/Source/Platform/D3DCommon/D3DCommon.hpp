#pragma once

#include "Fusion/IO/Logging.hpp"
#include "Fusion/Renderer/CommonTypes.hpp"

#include "D3DComPtr.hpp"

#include <dxgi1_6.h>

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

}
