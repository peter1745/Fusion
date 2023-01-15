#pragma once

#include "Fusion/World/World.hpp"
#include "Fusion/Serialization/YAMLCustomConverters.hpp"

#include <yaml-cpp/yaml.h>
#include <filesystem>

namespace Fusion {

	class WorldYAMLReader
	{
	public:
		WorldYAMLReader(const std::filesystem::path& InFilePath);
		~WorldYAMLReader() = default;

		void ReadWorld(Shared<World> InWorld);

		template<typename TValueType>
		TValueType Read(const std::string& InName) const
		{
			CoreVerify(static_cast<bool>(m_CurrentComponentNode));
			return m_CurrentComponentNode[InName].as<TValueType>();
		}

		template<typename TValueType>
		TValueType Read(const std::string& InName, const TValueType& InFallback) const
		{
			CoreVerify(static_cast<bool>(m_CurrentComponentNode));
			return m_CurrentComponentNode[InName].as<TValueType>(InFallback);
		}

	private:
		YAML::Node m_RootNode;
		YAML::Node m_CurrentComponentNode;
	};

}
