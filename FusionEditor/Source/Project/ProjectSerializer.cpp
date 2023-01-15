#include "ProjectSerializer.hpp"

#include "Fusion/IO/Logging.hpp"
#include "Fusion/Core/Assert.hpp"

#include <fstream>
#include <sstream>

#include <yaml-cpp/yaml.h>

namespace FusionEditor {

	void ProjectSerializer::Serialize(const std::shared_ptr<Project>& InProject)
	{
		YAML::Emitter Emitter;
		Emitter << YAML::BeginMap;
		Emitter << YAML::Key << "Project" << YAML::Value << InProject->Name;
		Emitter << YAML::EndMap;

		std::ofstream StreamOut(InProject->BaseDirectory / (InProject->Name + ".fproj"));
		StreamOut << Emitter.c_str();
		StreamOut.close();
	}

	std::shared_ptr<Project> ProjectSerializer::Deserialize(const std::filesystem::path& InFilePath)
	{
		std::ifstream StreamIn(InFilePath);
		Fusion::CoreVerify(StreamIn.good());
		std::stringstream StringBuffer;
		StringBuffer << StreamIn.rdbuf();

		YAML::Node RootNode = YAML::Load(StringBuffer);
		Fusion::CoreVerify(static_cast<bool>(RootNode));

		std::shared_ptr<Project> NewProject = std::make_shared<Project>();
		NewProject->Name = RootNode["Project"].as<std::string>();
		NewProject->BaseDirectory = InFilePath.parent_path();
		return NewProject;
	}

}
