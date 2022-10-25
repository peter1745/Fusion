#include "FusionPCH.hpp"
#include "FileIO.hpp"

namespace Fusion {

	bool FileIO::ReadFile(const std::filesystem::path& InPath, ByteBuffer& OutBuffer)
	{
		std::ifstream Stream(InPath, std::ios::ate | std::ios::binary);
		if (!Stream.is_open())
			return false;

		uint32_t StreamSize = uint32_t(Stream.tellg());

		if (StreamSize == 0)
			return false;

		OutBuffer.Allocate(StreamSize);

		Stream.seekg(0, std::ios::beg);
		Stream.read(reinterpret_cast<char*>(OutBuffer.Data), StreamSize);
		Stream.close();
		return true;
	}

	bool FileIO::ReadFileText(const std::filesystem::path& InPath, std::string& OutText)
	{
		std::ifstream Stream(InPath);
		if (!Stream.is_open())
			return false;

		std::stringstream StrStream;
		StrStream << Stream.rdbuf();
		OutText = StrStream.str();
		return true;
	}

}
