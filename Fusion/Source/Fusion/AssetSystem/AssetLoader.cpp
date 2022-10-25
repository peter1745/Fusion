#include "FusionPCH.hpp"
#include "AssetLoader.hpp"

#include "Fusion/Serialization/YAMLCustomConverters.hpp"

#include <yaml-cpp/yaml.h>

namespace Fusion {

	AssetContainer<Asset> AssetLoader::LoadFromFile_Internal(const std::filesystem::path& InFilePath)
	{
		std::ifstream StreamIn(InFilePath);
		if (!StreamIn)
			return nullptr;

		YAML::Node RootNode = YAML::Load(StreamIn);

		if (!RootNode)
		{
			FUSION_CORE_ERROR("Tried loading invalid asset from {}", InFilePath.string());
			return nullptr;
		}

		RootNode = RootNode.begin()->second;

		AssetHandle Handle = RootNode["Handle"].as<AssetHandle>(AssetHandle(0));
		EAssetType Type = Handle.GetType();



		return nullptr;
	}

}
