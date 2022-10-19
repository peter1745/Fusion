#include "FusionPCH.h"
#include "WorldSerializer.h"

#include "WorldWriter.h"
#include "WorldReader.h"

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
