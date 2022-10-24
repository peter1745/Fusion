#pragma once

#include "Windows/EditorWindow.h"

#include <filesystem>

namespace FusionEditor {

	class AssetImporterWindow : public EditorWindow
	{
	public:
		AssetImporterWindow(const std::string& InTitle)
			: EditorWindow(InTitle) {}

		void SetImportPaths(const std::filesystem::path& InSourcePath, const std::filesystem::path& InAssetOutputPath)
		{
			m_SourceAssetPath = InSourcePath;
			m_AssetOutputPath = InAssetOutputPath;
		}

	protected:
		std::filesystem::path m_SourceAssetPath;
		std::filesystem::path m_AssetOutputPath;
	};

}
