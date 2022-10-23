#pragma once

#include <Fusion/AssetSystem/Asset.h>

#include <filesystem>
#include <concepts>

namespace FusionEditor {

	template<typename TAsset>
	class AssetFactory
	{
	public:
		virtual Fusion::Shared<TAsset> ImportFromFile(const std::filesystem::path& InOutputPath, const std::filesystem::path& InFilePath) = 0;

	};

}
