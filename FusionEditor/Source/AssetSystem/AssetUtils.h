#pragma once

#include <Fusion/AssetSystem/Asset.h>

namespace FusionEditor {

	class AssetUtils
	{
	public:
		static Fusion::EAssetType GetTypeFromExtension(const std::filesystem::path& InFilePath);
	};

}
