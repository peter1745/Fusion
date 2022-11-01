#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>

#include "Fusion/Renderer/CommonTypes.hpp"
#include "D3D12ComPtr.hpp"

#define FUSION_RELEASE_D3D_RESOURCE(InResource) if (InResource != nullptr) { InResource->Release(); InResource = nullptr; }

namespace Fusion {

	static constexpr DXGI_FORMAT EGraphicsFormatToDXGIFormat(EGraphicsFormat InFormat)
	{
		switch (InFormat)
		{
		case EGraphicsFormat::Unknown: return DXGI_FORMAT_UNKNOWN;
		case EGraphicsFormat::RGBA32Float: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case EGraphicsFormat::RGBA32UInt: return DXGI_FORMAT_R32G32B32A32_UINT;
		case EGraphicsFormat::RGB32Float: return DXGI_FORMAT_R32G32B32_FLOAT;
		case EGraphicsFormat::RGB32UInt: return DXGI_FORMAT_R32G32B32_UINT;
		case EGraphicsFormat::RG32Float: return DXGI_FORMAT_R32G32_FLOAT;
		case EGraphicsFormat::RG32UInt: return DXGI_FORMAT_R32G32_UINT;
		case EGraphicsFormat::R32Float: return DXGI_FORMAT_R32_FLOAT;
		case EGraphicsFormat::R32UInt: return DXGI_FORMAT_R32_UINT;
		case EGraphicsFormat::RGBA8Unorm: return DXGI_FORMAT_R8G8B8A8_UNORM;
		case EGraphicsFormat::RGBA8UInt: return DXGI_FORMAT_R8G8B8A8_UINT;
		case EGraphicsFormat::D24UnormS8UInt: return DXGI_FORMAT_D24_UNORM_S8_UINT;
		}

		return DXGI_FORMAT_UNKNOWN;
	}

}
