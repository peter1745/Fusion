#pragma once

#include "ImmutableBuffer.hpp"

namespace Fusion {

	class FileIO
	{
	public:
		static bool ReadFile(const std::filesystem::path& InPath, ImmutableBuffer& OutBuffer);
		static bool ReadFileText(const std::filesystem::path& InPath, std::string& OutText);

	};

}
