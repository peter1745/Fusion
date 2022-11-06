#pragma once

#include "Platform/D3DCommon/D3DCommon.hpp"

#include <d3d12.h>

namespace Fusion {

	static constexpr D3D12_RESOURCE_FLAGS ImageFlagsToD3D12ResourceFlags(EImageFlag InFlags)
	{
		uint64_t Result = 0;

		if (InFlags & ImageFlags::AllowRenderTarget) Result |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		if (InFlags & ImageFlags::AllowDepthStencil) Result |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		return static_cast<D3D12_RESOURCE_FLAGS>(Result);
	}

	static constexpr D3D12_RESOURCE_STATES ImageStatesToD3D12ResourceStates(EImageState InStates)
	{
		uint64_t Result = 0;

		if (InStates & ImageStates::RenderTarget) Result |= D3D12_RESOURCE_STATE_RENDER_TARGET;
		if (InStates & ImageStates::DepthWrite) Result |= D3D12_RESOURCE_STATE_DEPTH_WRITE;
		if (InStates & ImageStates::DepthRead) Result |= D3D12_RESOURCE_STATE_DEPTH_READ;
		if (InStates & ImageStates::UnorderedAccess) Result |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		if (InStates & ImageStates::NonPixelShaderResource) Result |= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		if (InStates & ImageStates::PixelShaderResource) Result |= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		if (InStates & ImageStates::IndirectArgument) Result |= D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
		if (InStates & ImageStates::CopyDst) Result |= D3D12_RESOURCE_STATE_COPY_DEST;
		if (InStates & ImageStates::CopySrc) Result |= D3D12_RESOURCE_STATE_COPY_SOURCE;
		if (InStates & ImageStates::ResolveDst) Result |= D3D12_RESOURCE_STATE_RESOLVE_DEST;
		if (InStates & ImageStates::ResolveSrc) Result |= D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
		if (InStates & ImageStates::ShadingRateSrc) Result |= D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE;

		return static_cast<D3D12_RESOURCE_STATES>(Result);
	}

}
