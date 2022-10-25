#pragma once

#include "SharedAsset.hpp"

#include "Fusion/Core/UUID.hpp"

#include "Fusion/Renderer/Mesh.hpp"

namespace Fusion {

	enum class EAssetType { Mesh };

	class AssetHandle
	{
	public:
		AssetHandle() = default;
		AssetHandle(uint64_t InHandle);
		AssetHandle(EAssetType InType);
		AssetHandle(EAssetType InType, uint64_t InID);
		AssetHandle(const AssetHandle& InOther);

		operator uint64_t() { return m_Value; }
		operator const uint64_t() const { return m_Value; }

		uint64_t GetID() const { return m_Value & 0xFFFF'FFFF'FFFFULL; }
		EAssetType GetType() const { return static_cast<EAssetType>(m_Value >> 48); }

	private:
		void SetValue(EAssetType InType, uint64_t InID);

	private:
		// NOTE(Peter): Upper 16 bits is the asset type, the rest 48 bits are the randomly generated ID
		uint64_t m_Value = 0;
	};

	class Asset : public SharedAsset
	{
	public:
		Asset()
			: Handle(GetType())
		{}
		virtual ~Asset() = default;

		virtual EAssetType GetType() const { FUSION_CORE_VERIFY(false); }

	public:
		AssetHandle Handle;
	};

	template<typename T>
	requires std::derived_from<T, Asset>
	using AssetContainer = Shared<T, SharedAsset>;

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

namespace std {

	template<>
	struct hash<Fusion::AssetHandle>
	{
		size_t operator()(const Fusion::AssetHandle& InUUID) const noexcept
		{
			return (uint64_t)InUUID;
		}
	};

}
