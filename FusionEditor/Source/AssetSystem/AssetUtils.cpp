#include "AssetUtils.h"

#include <unordered_map>

namespace FusionEditor {

	static std::unordered_map<std::string, Fusion::EAssetType> s_AssetExtensionMap = {
		{ ".gltf", Fusion::EAssetType::Mesh },
		{ ".glb", Fusion::EAssetType::Mesh }
	};

	Fusion::EAssetType AssetUtils::GetTypeFromExtension(const std::filesystem::path& InFilePath)
	{
		auto It = s_AssetExtensionMap.find(InFilePath.extension().string());
		return It != s_AssetExtensionMap.end() ? It->second : Fusion::EAssetType::None;
	}

}
