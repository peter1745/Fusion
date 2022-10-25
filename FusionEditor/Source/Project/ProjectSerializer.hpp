#pragma once

#include "Project.hpp"

#include <memory>

namespace FusionEditor {

	class ProjectSerializer
	{
	public:
		static void Serialize(const std::shared_ptr<Project>& InProject);
		static std::shared_ptr<Project> Deserialize(const std::filesystem::path& InFilePath);
	};

}
