#pragma once

#include "Windows/EditorWindow.hpp"

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

		virtual constexpr EMenuBarLocation GetMenuBarLocation() const override { return EMenuBarLocation::None; }

	protected:
		std::filesystem::path m_SourceAssetPath;
		std::filesystem::path m_AssetOutputPath;
	};

}
