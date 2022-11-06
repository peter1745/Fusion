#pragma once

#include "Platform/D3DCommon/D3DCommon.hpp"

#include <d3d11.h>

namespace Fusion {

	static D3D11_USAGE BufferUsageToD3D11Usage(EBufferUsage InUsage)
	{
		switch (InUsage)
		{
		case EBufferUsage::Static: return D3D11_USAGE_DEFAULT;
		case EBufferUsage::Dynamic: return D3D11_USAGE_DYNAMIC;
		case EBufferUsage::Immutable: return D3D11_USAGE_IMMUTABLE;
		}

		FUSION_CORE_VERIFY(false);
		return D3D11_USAGE_DEFAULT;
	}

}
