#include "FusionPCH.hpp"
#include "AssetLoader.hpp"

#include "Fusion/Serialization/YAMLCustomConverters.hpp"

#include "Fusion/IO/FileIO.hpp"

#include "Loaders/MeshAssetLoader.hpp"

#include <yaml-cpp/yaml.h>

namespace Fusion {

	void AssetLoader::RegisterDefaultLoaders()
	{
		s_RegisteredLoaders[EAssetType::Mesh] = MakeUnique<MeshAssetLoader>();
	}

	AssetContainer<Asset> AssetLoader::LoadFromFile_Internal(const std::filesystem::path& InFilePath)
	{
		Fusion::ImmutableBuffer Buffer;
		if (!Fusion::FileIO::ReadFile(InFilePath, Buffer))
			return nullptr;

		Fusion::AssetHandle Handle = Buffer.Read<Fusion::AssetHandle>();
		EAssetType Type = Handle.GetType();
		CoreVerify(s_RegisteredLoaders.find(Type) != s_RegisteredLoaders.end());

		AssetContainer<Asset> Result = s_RegisteredLoaders.at(Type)->LoadAsset(Buffer);
		Result->Handle = Handle;
		return Result;
	}

}
