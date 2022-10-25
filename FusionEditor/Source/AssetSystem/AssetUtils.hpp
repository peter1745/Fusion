#pragma once

#include <Fusion/AssetSystem/Asset.hpp>

namespace FusionEditor {

	class AssetUtils
	{
	public:
		static Fusion::EAssetType GetTypeFromExtension(const std::filesystem::path& InFilePath);
	};

}
