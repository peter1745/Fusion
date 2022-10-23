#pragma once

#include "AssetSystem/AssetImporter.h"

namespace FusionEditor {

	class MeshAssetFactory : public AssetFactory<Fusion::MeshAsset>
	{
	public:
		virtual Fusion::Shared<Fusion::MeshAsset> ImportFromFile(const std::filesystem::path& InOutputPath, const std::filesystem::path& InFilePath) override;

	};

}
