#pragma once

#include "Fusion/Core/Core.h"

#include <filesystem>

namespace Fusion {

	class Texture2D
	{
	public:
		virtual ~Texture2D() = default;
		
		virtual void Bind(uint32_t InSlot) = 0;

	public:
		static Shared<Texture2D> LoadFromFile(const std::filesystem::path& InFilePath);
	};

}
