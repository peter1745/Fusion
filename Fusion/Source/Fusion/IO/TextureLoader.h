#pragma once

#include "Fusion/Renderer/Texture.h"

#include <filesystem>

namespace Fusion {

	class TextureLoader
	{
	public:
		static Shared<Texture2D> LoadFromFile(const std::filesystem::path& InFilePath);
	};

}
