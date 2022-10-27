#pragma once

#include "Asset.hpp"
#include "Fusion/IO/ImmutableBuffer.hpp"

namespace Fusion {

	class AssetLoaderImpl
	{
	public:
		virtual ~AssetLoaderImpl() = default;
		virtual AssetContainer<Asset> LoadAsset(ImmutableBuffer& InBuffer) = 0;
	};

	class AssetLoader
	{
	public:
		static void RegisterDefaultLoaders();

		/*template<typename TLoader>
		static void RegisterLoader(EAssetType InType)
		{
			FUSION_CORE_VERIFY(s_RegisteredLoaders.find(InType) != s_RegisteredLoaders.end());
			s_RegisteredLoaders[InType] = MakeUnique<TLoader>();
		}*/

		template<AssetType T>
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
