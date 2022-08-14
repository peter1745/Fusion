#pragma once

#include "ByteBuffer.h"

namespace Fusion {

	class FileIO
	{
	public:
		static bool ReadFile(const std::filesystem::path& InPath, ByteBuffer& OutBuffer);

	};

}
