#pragma once

#include "Asset.hpp"

#include <yaml-cpp/yaml.h>

namespace Fusion {

	class AssetLoaderImpl
	{
	public:
		virtual bool LoadAssetFromYAML(const YAML::Node& InNode) = 0;
	};

	class AssetLoader
	{
	public:
		template<typename T>
		static AssetContainer<T> LoadFromFile(const std::filesystem::path& InFilePath)
		{
			return LoadFromFile_Internal(InFilePath).As<T>();
		}

	private:
		static AssetContainer<Asset> LoadFromFile_Internal(const std::filesystem::path& InFilePath);

	private:
		inline static std::unordered_map<EAssetType, Unique<AssetLoaderImpl>> s_RegisteredLoaders;
	};

}
