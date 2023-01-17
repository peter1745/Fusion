#pragma once

#include "Image.hpp"

namespace Fusion {

	struct Texture2DInfo
	{
		ImageSize Size;
		uint32_t Channels;
		EFormat Format;
		Byte* Data;
	};

	class Texture2D : public SharedObject
	{
	public:
		Texture2D(const Texture2DInfo& InInfo);

		VkImageView GetImageView() const { return m_ImageView; }
		VkSampler GetSampler() const { return m_Sampler; }

	private:
		Texture2DInfo m_CreateInfo;
		Shared<Image2D> m_Image = nullptr;
		VkImageView m_ImageView = VK_NULL_HANDLE;
		VkSampler m_Sampler = VK_NULL_HANDLE;
	};

}
