#include "FusionPCH.hpp"
#include "Image.hpp"
#include "Device.hpp"
#include "GraphicsContext.hpp"

#include "Fusion/Renderer/Renderer.hpp"

namespace Fusion {

	Image2D::Image2D(const Image2DInfo& InCreateInfo)
	    : m_CreateInfo(InCreateInfo)
	{
		m_State = m_CreateInfo.InitialState;

		Invalidate();
	}

	void Image2D::Transition(CommandBuffer* InCmdList, EImageState InState)
	{
		if (m_State == InState)
			return;

		ImageTransitionInfo TransitionInfo = {};
		TransitionInfo.Image = m_Image;
		TransitionInfo.OldLayout = ImageStatesToVkImageLayout(m_State);
		TransitionInfo.NewLayout = ImageStatesToVkImageLayout(InState);
		TransitionInfo.SrcAccessFlag = 0;
		TransitionInfo.DstAccessFlag = 0;
		TransitionInfo.SrcStage = 0;
		TransitionInfo.DstStage = 0;
		TransitionInfo.AspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
		TransitionImage(InCmdList->GetBuffer(), TransitionInfo);

		m_State = InState;
	}

	void Image2D::CopyTo(CommandBuffer* InCmdList, const ImageRegion& InRegion, Buffer* InBuffer)
	{
		VkBufferImageCopy CopyRegion;
		CopyRegion.bufferOffset = 0;
		CopyRegion.bufferRowLength = 0;
		CopyRegion.bufferImageHeight = 0;
		CopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		CopyRegion.imageSubresource.layerCount = 1;
		CopyRegion.imageSubresource.baseArrayLayer = 0;
		CopyRegion.imageSubresource.mipLevel = 0;
		CopyRegion.imageOffset = { InRegion.X, InRegion.Y, 0 };
		CopyRegion.imageExtent.width = InRegion.Width;
		CopyRegion.imageExtent.height = InRegion.Height;
		CopyRegion.imageExtent.depth = 1;

		vkCmdCopyImageToBuffer(InCmdList->GetBuffer(), m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, InBuffer->GetBuffer(), 1, &CopyRegion);
	}

	void Image2D::CopyFrom(CommandBuffer* InCmdList, const ImageRegion& InRegion, Buffer* InBuffer)
	{
		VkBufferImageCopy CopyRegion;
		CopyRegion.bufferOffset = 0;
		CopyRegion.bufferRowLength = 0;
		CopyRegion.bufferImageHeight = 0;
		CopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		CopyRegion.imageSubresource.layerCount = 1;
		CopyRegion.imageSubresource.baseArrayLayer = 0;
		CopyRegion.imageSubresource.mipLevel = 0;
		CopyRegion.imageOffset = { InRegion.X, InRegion.Y, 0 };
		CopyRegion.imageExtent.width = InRegion.Width;
		CopyRegion.imageExtent.height = InRegion.Height;
		CopyRegion.imageExtent.depth = 1;

		vkCmdCopyBufferToImage(InCmdList->GetBuffer(), InBuffer->GetBuffer(), m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &CopyRegion);
	}

	void Image2D::Invalidate()
	{
		VkImageCreateInfo ImageCreateInfo = {};
		ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		ImageCreateInfo.extent.width = m_CreateInfo.Size.Width;
		ImageCreateInfo.extent.height = m_CreateInfo.Size.Height;
		ImageCreateInfo.extent.depth = 1;
		ImageCreateInfo.mipLevels = 1;
		ImageCreateInfo.arrayLayers = 1;
		ImageCreateInfo.format = EFormatToVkFormat(m_CreateInfo.Format);
		ImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		ImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		ImageCreateInfo.usage = GetImageUsageFlags(m_CreateInfo.Usage, m_CreateInfo.Format);
		ImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		ImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		ImageCreateInfo.flags = 0;

		m_Allocation = GraphicsContext::Get()->GetAllocator()->CreateImage(ImageCreateInfo, &m_Image);
	}

	void Image2D::Release()
	{
		/*Renderer::GetCurrent().SubmitResourceForDestruction(3, [InAllocation = m_Allocation, InImage = m_Image]()
	    {
		    auto* Allocator = GraphicsContext::Get<GraphicsContext>()->GetAllocator();
		    Allocator->DestroyImage(InAllocation, InImage);
	    });*/
	}

}
