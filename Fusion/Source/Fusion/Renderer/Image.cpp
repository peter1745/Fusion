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

		VkImageMemoryBarrier Barrier = {};
		Barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		Barrier.oldLayout = ImageStatesToVkImageLayout(m_State);
		Barrier.newLayout = ImageStatesToVkImageLayout(InState);
		Barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		Barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		Barrier.image = m_Image;
		Barrier.subresourceRange.aspectMask = IsDepthFormat(m_CreateInfo.Format) ? (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT) : VK_IMAGE_ASPECT_COLOR_BIT;
		Barrier.subresourceRange.baseMipLevel = 0;
		Barrier.subresourceRange.levelCount = 1;
		Barrier.subresourceRange.baseArrayLayer = 0;
		Barrier.subresourceRange.layerCount = 1;
		Barrier.srcAccessMask = 0;
		Barrier.dstAccessMask = 0;

		vkCmdPipelineBarrier(InCmdList->GetBuffer(), 0, 0, 0, 0, nullptr, 0, nullptr, 1, &Barrier);

		m_State = InState;
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
