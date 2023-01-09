#include "FusionPCH.hpp"
#include "MeshAssetLoader.hpp"
#include "Fusion/Serialization/YAMLCustomConverters.hpp"
#include "Fusion/Renderer/GraphicsContext.hpp"

namespace Fusion {

	AssetContainer<Asset> MeshAssetLoader::LoadAsset(ImmutableBuffer& InBuffer)
	{
		size_t VertexCount = InBuffer.Read<size_t>();
		std::vector<Fusion::Vertex> Vertices;
		Vertices.reserve(VertexCount);
		for (size_t Idx = 0; Idx < VertexCount; Idx++)
			Vertices.push_back(InBuffer.Read<Fusion::Vertex>());

		size_t IndexCount = InBuffer.Read<size_t>();
		std::vector<Fusion::Index> Indices;
		Indices.reserve(IndexCount);
		for (size_t Idx = 0; Idx < IndexCount; Idx++)
			Indices.push_back(InBuffer.Read<Fusion::Index>());

		return AssetContainer<MeshAsset>::Create(Vertices, Indices);
	}

}
