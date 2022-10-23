#include "MeshAssetFactory.h"

#include <Fusion/IO/GLTFLoader.h>
#include <Fusion/Renderer/Mesh.h>

#include <Fusion/Serialization/YAMLCustomConverters.h>
#include <yaml-cpp/yaml.h>

#include <fstream>

namespace FusionEditor {

	static bool HasExtension(const std::filesystem::path& InFilePath, const char* InExtension)
	{
		return InFilePath.extension().string().find(InExtension) != std::string::npos;
	}
		
	Fusion::Shared<Fusion::MeshAsset> MeshAssetFactory::ImportFromFile(const std::filesystem::path& InOutputPath, const std::filesystem::path& InFilePath)
	{
		Fusion::Shared<Fusion::MeshAsset> Result = nullptr;

		if (HasExtension(InFilePath, "gltf") || HasExtension(InFilePath, "glb"))
		{
			Fusion::GLTFData Data;
			if (!Fusion::GLTFLoader::LoadGLTFMesh(InFilePath, Data))
				return nullptr;

			Result = Fusion::Shared<Fusion::MeshAsset>::Create(Data.Vertices, Data.Indices);

			YAML::Emitter Emitter;
			Emitter << YAML::BeginMap;
			Emitter << YAML::Key << "Mesh" << YAML::Value << YAML::BeginMap;
			Emitter << YAML::Key << "Handle" << YAML::Value << Result->Handle;
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

			std::ofstream StreamOut(InOutputPath);
			StreamOut << Emitter.c_str();
			StreamOut.close();
		}

		return Result;
	}

}
