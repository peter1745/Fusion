#include "FusionPCH.h"
#include "WorldSerializer.h"

#include "WorldWriter.h"

namespace Fusion {

	void WorldSerializer::SerializeWorld(const std::filesystem::path& InFilePath, const Shared<World>& InWorld)
	{
		WorldYAMLWriter Writer;
		Writer.WriteWorld(InWorld);
		Writer.Close(InFilePath);
	}

}
