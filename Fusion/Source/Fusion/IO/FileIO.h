#pragma once

#include "ByteBuffer.h"

namespace Fusion {

	class FileIO
	{
	public:
		static bool ReadFile(const std::filesystem::path& InPath, ByteBuffer& OutBuffer);
		static bool ReadFileText(const std::filesystem::path& InPath, std::string& OutText);

	};

}
