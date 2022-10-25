#include "MeshImporterWindow.hpp"

#include "Windows/WindowManager.hpp"

#include <Fusion/IO/GLTFLoader.hpp>
#include <Fusion/Renderer/Mesh.hpp>
#include <Fusion/AssetSystem/Asset.hpp>
#include <Fusion/AssetSystem/AssetLoader.hpp>

#include <Fusion/Serialization/YAMLCustomConverters.hpp>
#include <yaml-cpp/yaml.h>

#include <fstream>

namespace FusionEditor {

	static bool HasExtension(const std::filesystem::path& InFilePath, const char* InExtension)
	{
		return InFilePath.extension().string().find(InExtension) != std::string::npos;
	}

	MeshImporterWindow::MeshImporterWindow()
		: AssetImporterWindow("Import Mesh")
	{
	}

	void MeshImporterWindow::Import()
	{
		if (!HasExtension(m_SourceAssetPath, "gltf") && !HasExtension(m_SourceAssetPath, "glb"))
			return;

		Fusion::GLTFData Data;
		if (!Fusion::GLTFLoader::LoadGLTFMesh(m_SourceAssetPath, Data))
			return;

		YAML::Emitter Emitter;
		Emitter << YAML::BeginMap;
		Emitter << YAML::Key << "Mesh" << YAML::Value << YAML::BeginMap;
		Emitter << YAML::Key << "Handle" << YAML::Value << Fusion::AssetHandle(Fusion::EAssetType::Mesh);
		Emitter << YAML::Key << "Name" << YAML::Value << Data.Name;

		// Vertices
		{
			Emitter << YAML::Key << "Vertices" << YAML::Value << YAML::BeginMap;

			for (const auto& VertexData : Data.Vertices)
			{
				Emitter << YAML::Key << "Vertex" << YAML::BeginMap;
				Emitter << YAML::Key << "Position" << YAML::Value << VertexData.Position;
				Emitter << YAML::Key << "Normal" << YAML::Value << VertexData.Normal;
				Emitter << YAML::Key << "UV" << YAML::Value << VertexData.TextureCoordinate;
				Emitter << YAML::EndMap;
			}

			Emitter << YAML::EndMap;
		}

		// Indices
		{
			Emitter << YAML::Key << "Indices" << YAML::Value << YAML::BeginMap;

			for (const auto& IndicesData : Data.Indices)
			{
				Emitter << YAML::Key << "Index" << YAML::BeginMap;
				Emitter << YAML::Key << "V0" << YAML::Value << IndicesData.Vertex0;
				Emitter << YAML::Key << "V1" << YAML::Value << IndicesData.Vertex1;
				Emitter << YAML::Key << "V2" << YAML::Value << IndicesData.Vertex2;
				Emitter << YAML::EndMap;
			}

			Emitter << YAML::EndMap;
		}

		Emitter << YAML::EndMap;
		Emitter << YAML::EndMap;

		std::ofstream StreamOut(m_AssetOutputPath / m_SourceAssetPath.filename().replace_extension("fmesh"));
		StreamOut << Emitter.c_str();
		StreamOut.close();

		Fusion::AssetLoader::LoadFromFile<Fusion::MeshAsset>(m_AssetOutputPath / m_SourceAssetPath.filename().replace_extension("fmesh"));
	}

	void MeshImporterWindow::RenderContents()
	{
		if (ImGui::Button("Import"))
		{
			Import();
			WindowManager::Get()->CloseWindowByID(GetID());
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel"))
			WindowManager::Get()->CloseWindowByID(GetID());
	}

}
