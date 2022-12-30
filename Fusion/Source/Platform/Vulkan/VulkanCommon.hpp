#pragma once

#include "Fusion/IO/Logging.hpp"
#include "Fusion/Renderer/CommonTypes.hpp"

#include <vulkan/vulkan.h>

namespace Fusion {

	static constexpr VkFormat EFormatToVkFormat(EFormat InFormat)
	{
		switch (InFormat)
		{
		case EFormat::Unknown: return VK_FORMAT_UNDEFINED;
		case EFormat::RGBA32Float: return VK_FORMAT_R32G32B32A32_SFLOAT;
		case EFormat::RGBA32UInt: return VK_FORMAT_R32G32B32A32_UINT;
		case EFormat::RGB32Float: return VK_FORMAT_R32G32B32_SFLOAT;
		case EFormat::RGB32UInt: return VK_FORMAT_R32G32B32_UINT;
		case EFormat::RG32Float: return VK_FORMAT_R32G32_SFLOAT;
		case EFormat::RG32UInt: return VK_FORMAT_R32G32_UINT;
		case EFormat::R32Float: return VK_FORMAT_R32_SFLOAT;
		case EFormat::R32UInt: return VK_FORMAT_R32_UINT;
		case EFormat::RGBA8Unorm: return VK_FORMAT_R8G8B8A8_UNORM;
		case EFormat::RGBA8UInt: return VK_FORMAT_R8G8B8A8_UINT;
		case EFormat::D24UnormS8UInt: return VK_FORMAT_D24_UNORM_S8_UINT;
		}

		return VK_FORMAT_UNDEFINED;
	}

	static constexpr VkImageUsageFlags GetImageUsageFlags(EImageUsage InUsage, EFormat InFormat)
	{
		VkImageUsageFlags Result = 0;

		if (InUsage == EImageUsage::Attachment)
		{
			if (IsDepthFormat(InFormat))
				Result |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			else
				Result |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		}
		else
		{
			Result = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}

		Result |= VK_IMAGE_USAGE_SAMPLED_BIT;

		return Result;
	}

	static constexpr VkImageLayout ImageStatesToVkImageLayout(EImageState InStates)
	{
		uint64_t Result = 0;
		
		if (InStates & ImageStates::RenderTarget) Result |= VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
		//if (InStates & ImageStates::DepthWrite) Result |= D3D12_RESOURCE_STATE_DEPTH_WRITE;
		//if (InStates & ImageStates::DepthRead) Result |= D3D12_RESOURCE_STATE_DEPTH_READ;
		//if (InStates & ImageStates::UnorderedAccess) Result |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		//if (InStates & ImageStates::NonPixelShaderResource) Result |= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		if (InStates & ImageStates::PixelShaderResource) Result |= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		//if (InStates & ImageStates::IndirectArgument) Result |= D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
		//if (InStates & ImageStates::CopyDst) Result |= D3D12_RESOURCE_STATE_COPY_DEST;
		//if (InStates & ImageStates::CopySrc) Result |= D3D12_RESOURCE_STATE_COPY_SOURCE;
		//if (InStates & ImageStates::ResolveDst) Result |= D3D12_RESOURCE_STATE_RESOLVE_DEST;
		//if (InStates & ImageStates::ResolveSrc) Result |= D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
		//if (InStates & ImageStates::ShadingRateSrc) Result |= D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE;

		return static_cast<VkImageLayout>(Result);
	}
}
