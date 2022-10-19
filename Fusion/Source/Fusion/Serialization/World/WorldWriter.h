#pragma once

#include "Fusion/World/World.h"

#include <yaml-cpp/yaml.h>
#include <filesystem>

namespace Fusion {

	class WorldYAMLWriter
	{
	public:
		WorldYAMLWriter() = default;
		~WorldYAMLWriter() = default;

		void WriteWorld(const Shared<World>& InWorld);
		void Write(const char* InName, const auto& InValue);
		
		void Close(const std::filesystem::path& InFilePath);

	private:
		YAML::Emitter m_Emitter;
	};

}
