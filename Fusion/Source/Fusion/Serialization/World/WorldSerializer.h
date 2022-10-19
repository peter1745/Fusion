#pragma once

#include "Fusion/World/World.h"

#include <filesystem>

namespace Fusion {

	class WorldSerializer
	{
	public:
		static void SerializeWorld(const std::filesystem::path& InFilePath, const Shared<World>& InWorld);
		static void DeserializeWorld(const std::filesystem::path& InFilePath, Shared<World> InWorld);
	};

}
