#include "FusionPCH.hpp"
#include "Texture.hpp"
#include "GraphicsContext.hpp"
#include "Buffer.hpp"

#include "Renderer.hpp"

namespace Fusion {

	Texture2D::Texture2D(const Texture2DInfo& InInfo)
	    : m_CreateInfo(InInfo)
	{
		Image2DInfo ImageInfo = {};
		ImageInfo.Size = InInfo.Size;
		ImageInfo.Usage = EImageUsage::Texture | EImageUsage::CopyDestination;
		ImageInfo.Format = InInfo.Format;
		ImageInfo.Channels = InInfo.Channels;
		m_Image = Shared<Image2D>::Create(ImageInfo);

		// Upload Data
		uint32_t BufferSize = InInfo.Size.Width * InInfo.Size.Height * InInfo.Channels;
		BufferInfo StagingBufferInfo = {};
		StagingBufferInfo.Usage = EBufferUsage::CopySource;
		StagingBufferInfo.Size = BufferSize;
		Shared<Buffer> StagingBuffer = Shared<Buffer>::Create(StagingBufferInfo);
		void* Data = StagingBuffer->Map();
		memcpy(Data, InInfo.Data, BufferSize);
		StagingBuffer->Unmap();

		Renderer::GetCurrent().ExecuteImmediate([&](CommandBuffer* InCommandBuffer)
		{
			m_Image->Transition(InCommandBuffer, EImageState::CopyDestination);

			ImageRegion Region = {};
			Region.X = 0;
			Region.Y = 0;
			Region.Width = InInfo.Size.Width;
			Region.Height = InInfo.Size.Height;
			m_Image->CopyFrom(InCommandBuffer, Region, StagingBuffer.Get());
		});

		VkImageViewCreateInfo ImageViewInfo = {};
		ImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ImageViewInfo.pNext = nullptr;
		ImageViewInfo.flags = 0;
		ImageViewInfo.image = m_Image->GetImage();
		ImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ImageViewInfo.format = EFormatToVkFormat(InInfo.Format);
		ImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ImageViewInfo.subresourceRange.baseMipLevel = 0;
		ImageViewInfo.subresourceRange.levelCount = 1;
		ImageViewInfo.subresourceRange.baseArrayLayer = 0;
		ImageViewInfo.subresourceRange.layerCount = 1;
		vkCreateImageView(GraphicsContext::Get()->GetDevice()->GetLogicalDevice(), &ImageViewInfo, nullptr, &m_ImageView);

		VkSamplerCreateInfo SamplerInfo = {};
		SamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		SamplerInfo.pNext = nullptr;
		SamplerInfo.flags = 0;
		SamplerInfo.magFilter = VK_FILTER_LINEAR;
		SamplerInfo.minFilter = VK_FILTER_LINEAR;
		SamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		SamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		SamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		SamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		SamplerInfo.mipLodBias = 0.0f;
		SamplerInfo.anisotropyEnable = VK_FALSE;
		SamplerInfo.maxAnisotropy = 0.0f;
		SamplerInfo.compareEnable = VK_FALSE;
		SamplerInfo.compareOp = VK_COMPARE_OP_LESS;
		SamplerInfo.minLod = 0.0f;
		SamplerInfo.maxLod = 0.0f;
		SamplerInfo.unnormalizedCoordinates = VK_FALSE;
		vkCreateSampler(GraphicsContext::Get()->GetDevice()->GetLogicalDevice(), &SamplerInfo, nullptr, &m_Sampler);

		Renderer::GetCurrent().ExecuteImmediate([&](CommandBuffer* InCommandBuffer)
		{
			m_Image->Transition(InCommandBuffer, EImageState::PixelShaderResource);
		});
	}

}
