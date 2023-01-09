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

	static constexpr VkShaderStageFlags ShaderTypeToVkShaderStageFlags(EShaderType InShaderStage)
	{
		switch (InShaderStage)
		{
		case EShaderType::Vertex: return VK_SHADER_STAGE_VERTEX_BIT;
		case EShaderType::Pixel: return VK_SHADER_STAGE_FRAGMENT_BIT;
		}

		return VK_SHADER_STAGE_ALL;
	}

	static constexpr VkBufferUsageFlags BufferUsageToVkBufferUsageFlags(EBufferUsage InUsage)
	{
		uint32_t Result = 0;

		if (InUsage & BufferUsage::Vertex) Result |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		if (InUsage & BufferUsage::Index) Result |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		if (InUsage & BufferUsage::CopyDestination) Result |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		if (InUsage & BufferUsage::CopySource) Result |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		return static_cast<VkBufferUsageFlags>(Result);
	}

	static constexpr VkImageLayout ImageStatesToVkImageLayout(EImageState InStates)
	{
		if (InStates & ImageStates::RenderTarget) return VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
		if (InStates & ImageStates::DepthWrite) return VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
		if (InStates & ImageStates::DepthRead) return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		if (InStates & ImageStates::PixelShaderResource) return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		return VK_IMAGE_LAYOUT_UNDEFINED;
	}

	struct ImageTransitionInfo
	{
		VkImage Image;
		VkImageLayout OldLayout;
		VkImageLayout NewLayout;
		VkAccessFlags SrcAccessFlag;
		VkAccessFlags DstAccessFlag;
		VkPipelineStageFlags SrcStage;
		VkPipelineStageFlags DstStage;
		VkImageAspectFlags AspectFlags;
	};

	static void TransitionImage(VkCommandBuffer InCommandBuffer, const ImageTransitionInfo& InTransitionInfo)
	{
		VkImageMemoryBarrier Barrier = {};
		Barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		Barrier.oldLayout = InTransitionInfo.OldLayout;
		Barrier.newLayout = InTransitionInfo.NewLayout;
		Barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		Barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		Barrier.image = InTransitionInfo.Image;
		Barrier.subresourceRange.aspectMask = InTransitionInfo.AspectFlags;
		Barrier.subresourceRange.baseMipLevel = 0;
		Barrier.subresourceRange.levelCount = 1;
		Barrier.subresourceRange.baseArrayLayer = 0;
		Barrier.subresourceRange.layerCount = 1;
		Barrier.srcAccessMask = InTransitionInfo.SrcAccessFlag;
		Barrier.dstAccessMask = InTransitionInfo.DstAccessFlag;

		vkCmdPipelineBarrier(InCommandBuffer, InTransitionInfo.SrcStage, InTransitionInfo.DstStage, 0, 0, nullptr, 0, nullptr, 1, &Barrier);
	}
}
