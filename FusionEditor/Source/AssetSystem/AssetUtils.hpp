#pragma once

#include <Fusion/AssetSystem/Asset.hpp>

namespace FusionEditor {

	class AssetUtils
	{
	public:
		static Fusion::EAssetType GetTypeFromExtension(const std::filesystem::path& InFilePath);
		static Fusion::AssetHandle GetHandleFromFilePath(const std::filesystem::path& InFilePath);

	};

}
