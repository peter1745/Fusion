#pragma once

#include <filesystem>

namespace FusionEditor {

	struct Project
	{
		std::filesystem::path BaseDirectory;
		std::string Name;
	};

}
