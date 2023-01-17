#include "FusionPCH.hpp"
#include "TextureLoader.hpp"

#include <stb_image/stb_image.h>

namespace Fusion {

	Shared<Texture2D> TextureLoader::LoadFromFile(const std::filesystem::path& InFilePath)
	{
		int32_t ImageWidth = 0;
		int32_t ImageHeight = 0;
		int32_t ImageChannels = 0;
		stbi_uc* Data = stbi_load(InFilePath.string().c_str(), &ImageWidth, &ImageHeight, &ImageChannels, 4);

		if (Data == nullptr || ImageWidth == 0 || ImageHeight == 0 || ImageChannels == 0)
		{
			LogInfo("Fusion", "Failed to load texture '{}'!", InFilePath.string());
			return nullptr;
		}

		Texture2DInfo TextureInfo = {};
		TextureInfo.Size.Width = static_cast<uint32_t>(ImageWidth);
		TextureInfo.Size.Height = static_cast<uint32_t>(ImageHeight);
		TextureInfo.Format = EFormat::RGBA8Unorm;
		TextureInfo.Data = Data;
		TextureInfo.Channels = static_cast<uint32_t>(ImageChannels);
		return Shared<Texture2D>::Create(TextureInfo);
	}

}
