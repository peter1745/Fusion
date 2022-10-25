#include "FusionPCH.hpp"
#include "WorldSerializer.hpp"

#include "WorldWriter.hpp"
#include "WorldReader.hpp"

namespace Fusion {

	void WorldSerializer::SerializeWorld(const std::filesystem::path& InFilePath, const Shared<World>& InWorld)
	{
		WorldYAMLWriter Writer;
		Writer.WriteWorld(InWorld);
		Writer.Close(InFilePath);
	}

	void WorldSerializer::DeserializeWorld(const std::filesystem::path& InFilePath, Shared<World> InWorld)
	{
		WorldYAMLReader Reader(InFilePath);
		Reader.ReadWorld(InWorld);
	}

}
