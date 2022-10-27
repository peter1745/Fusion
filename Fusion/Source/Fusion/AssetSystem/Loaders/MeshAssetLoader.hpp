#pragma once

#include "Fusion/AssetSystem/AssetLoader.hpp"

namespace Fusion {

	class MeshAssetLoader : public AssetLoaderImpl
	{
	public:
		virtual AssetContainer<Asset> LoadAsset(ImmutableBuffer& InBuffer) override;
	};

}
