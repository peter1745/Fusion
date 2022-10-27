#include "AssetUtils.hpp"
#include <Fusion/Core/Application.hpp>

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

	static std::unordered_map<std::filesystem::path, Fusion::AssetHandle> s_AssetFilePath;

	Fusion::AssetHandle AssetUtils::GetHandleFromFilePath(const std::filesystem::path& InFilePath)
	{
		if (s_AssetFilePath.find(InFilePath) == s_AssetFilePath.end())
		{
			const auto& Databanks = Fusion::Application::Get().GetAssetStorage()->GetDatabanks();
			bool FoundDatabank = false;

			for (const auto& [Handle, Databank] : Databanks)
			{
				if (std::filesystem::equivalent(Databank.FilePath, InFilePath))
				{
					s_AssetFilePath[InFilePath] = Handle;
					FoundDatabank = true;
					break;
				}
			}

			if (!FoundDatabank)
				return 0;
		}

		return s_AssetFilePath.at(InFilePath);
	}

}
