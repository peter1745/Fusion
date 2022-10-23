#pragma once

#include "Fusion/Memory/Shared.h"
#include "Fusion/Core/UUID.h"

#include "Fusion/Renderer/Mesh.h"

namespace Fusion {

	enum class EAssetType
	{
		None = -1, Mesh
	};

	class Asset : public SharedObject
	{
	public:
		virtual ~Asset() = default;

		virtual EAssetType GetType() const { return EAssetType::None; }

	public:
		UUID Handle;
	};

	class MeshAsset : public Asset
	{
	public:
		MeshAsset(const std::vector<Vertex>& InVertices, const std::vector<Index>& InIndices)
			: m_Mesh(new Mesh(InVertices, InIndices)) {}

		virtual EAssetType GetType() const override { return EAssetType::Mesh; }

	private:
		Unique<Mesh> m_Mesh = nullptr;
	};

}
