#include "AssetUtils.hpp"

#include <unordered_map>

namespace FusionEditor {

	static std::unordered_map<std::string, Fusion::EAssetType> s_AssetExtensionMap = {
		{ ".gltf", Fusion::EAssetType::Mesh },
		{ ".glb", Fusion::EAssetType::Mesh }
	};

	Fusion::EAssetType AssetUtils::GetTypeFromExtension(const std::filesystem::path& InFilePath)
	{
		auto It = s_AssetExtensionMap.find(InFilePath.extension().string());
		FUSION_CORE_VERIFY(It != s_AssetExtensionMap.end());
		return It->second;
	}

}
