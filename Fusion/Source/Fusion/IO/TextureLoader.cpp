#include "FusionPCH.h"
#include "TextureLoader.h"

#include <stb_image/stb_image.h>

namespace Fusion {

	Shared<Texture2D> TextureLoader::LoadFromFile(const std::filesystem::path& InFilePath)
	{
		int32_t ImageWidth = 0;
		int32_t ImageHeight = 0;
		int32_t BitsPerPixel = 0;
		stbi_uc* Data = stbi_load(InFilePath.string().c_str(), &ImageWidth, &ImageHeight, &BitsPerPixel, 4);

		if (Data == nullptr || ImageWidth == 0 || ImageHeight == 0 || BitsPerPixel == 0)
		{
			FUSION_CORE_INFO("Failed to load texture '{}'!", InFilePath.string());
			return nullptr;
		}

		Texture2DInfo CreateInfo;
		CreateInfo.Data = Data;
		CreateInfo.Width = static_cast<uint32_t>(ImageWidth);
		CreateInfo.Height = static_cast<uint32_t>(ImageHeight);
		CreateInfo.BitsPerPixel = static_cast<uint32_t>(BitsPerPixel);

		Shared<Texture2D> Result = Texture2D::Create(CreateInfo);
		// TODO(Peter): Don't always do it
		//stbi_image_free(Data);
		return Result;
	}

}
