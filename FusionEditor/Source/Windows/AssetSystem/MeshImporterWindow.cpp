#include "MeshImporterWindow.hpp"

#include "Windows/WindowManager.hpp"

#include <Fusion/Core/Application.hpp>

#include <Fusion/IO/ImmutableBuffer.hpp>
#include <Fusion/IO/FileIO.hpp>
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

		Fusion::AssetHandle Handle(Fusion::EAssetType::Mesh);

		uint32_t VertexSize = uint32_t(Data.Vertices.size()) * sizeof(Fusion::Vertex);
		uint32_t IndexSize = uint32_t(Data.Indices.size()) * sizeof(Fusion::Index);
		Fusion::WritableBuffer Buffer(sizeof(Fusion::AssetHandle) + VertexSize + IndexSize + sizeof(size_t) + sizeof(size_t));

		Buffer.Write<uint64_t>(Handle);

		// Vertices
		Buffer.Write(Data.Vertices.size());
		for (const auto& VertexData : Data.Vertices)
			Buffer.Write(VertexData);

		// Indices
		Buffer.Write(Data.Indices.size());
		for (const auto& IndicesData : Data.Indices)
			Buffer.Write(IndicesData);

		std::filesystem::path OutputPath = m_AssetOutputPath / m_SourceAssetPath.filename().replace_extension("fasset");
		std::ofstream StreamOut(OutputPath, std::ios::binary | std::ios::trunc);
		StreamOut.write(reinterpret_cast<const char*>(Buffer.GetData()), Buffer.GetSize());
		StreamOut.close();
		Buffer.Release();

		// Create AssetDatabank
		Fusion::Application::Get().GetAssetStorage()->AddDatabank(Handle, { OutputPath });
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
