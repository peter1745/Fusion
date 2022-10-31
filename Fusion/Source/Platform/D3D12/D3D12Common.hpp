#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>

#include "D3D12ComPtr.hpp"

#define FUSION_RELEASE_D3D_RESOURCE(InResource) if (InResource != nullptr) { InResource->Release(); InResource = nullptr; }

namespace Fusion {

}
