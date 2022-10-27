#include "FusionPCH.hpp"
#include "GLTFLoader.hpp"

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
#include <TinyGLTF/tiny_gltf.h>

namespace Fusion {

	static bool HasExtension(const std::filesystem::path& InFilePath, const char* InExtension)
	{
		return InFilePath.extension().string().find(InExtension) != std::string::npos;
	}

	bool GLTFLoader::LoadGLTFMesh(const std::filesystem::path& InFilePath, GLTFData& OutData)
	{
		tinygltf::TinyGLTF Loader;
		tinygltf::Model ModelData;
		std::string WarningMessage, ErrorMessage;

		const bool IsBinaryFile = HasExtension(InFilePath, "glb");

		bool Loaded = false;

		if (IsBinaryFile)
			Loaded = Loader.LoadBinaryFromFile(&ModelData, &ErrorMessage, &WarningMessage, InFilePath.string());
		else
			Loaded = Loader.LoadASCIIFromFile(&ModelData, &ErrorMessage, &WarningMessage, InFilePath.string());

		if (!ErrorMessage.empty())
		{
			FUSION_CORE_ERROR("Error generated while loading mesh '{}'!", InFilePath.string());
			FUSION_CORE_ERROR("Error: {}", ErrorMessage);
			return false;
		}

		if (!WarningMessage.empty())
		{
			FUSION_CORE_WARN("Warning generated while loading mesh '{}'!", InFilePath.string());
			FUSION_CORE_WARN("Warning: {}", WarningMessage);
		}

		if (!Loaded)
		{
			FUSION_CORE_ERROR("Failed to load glTF file '{}'!", InFilePath.string());
			return false;
		}

		for (tinygltf::Mesh& MeshData : ModelData.meshes)
		{
			OutData.Name = MeshData.name;

			for (tinygltf::Primitive& Primitive : MeshData.primitives)
			{
				// NOTE(Peter): We really shouldn't assume that *every* mesh has normals or texture coords
				const bool HasNormals = Primitive.attributes.find("NORMAL") != Primitive.attributes.end();
				const bool HasTextureCoords = Primitive.attributes.find("TEXCOORD_0") != Primitive.attributes.end();

				const tinygltf::Accessor& PositionsAccessor = ModelData.accessors[Primitive.attributes.at("POSITION")];
				const tinygltf::BufferView& PositionsBufferView = ModelData.bufferViews[PositionsAccessor.bufferView];
				const tinygltf::Buffer& PositionsBuffer = ModelData.buffers[PositionsBufferView.buffer];
				const glm::vec3* VertexPositions = reinterpret_cast<const glm::vec3*>(&PositionsBuffer.data[PositionsBufferView.byteOffset + PositionsAccessor.byteOffset]);

				glm::vec3* VertexNormals = nullptr;
				if (HasNormals)
				{
					tinygltf::Accessor& NormalsAccessor = ModelData.accessors[Primitive.attributes.at("NORMAL")];
					tinygltf::BufferView& NormalsBufferView = ModelData.bufferViews[NormalsAccessor.bufferView];
					tinygltf::Buffer& NormalsBuffer = ModelData.buffers[NormalsBufferView.buffer];
					VertexNormals = reinterpret_cast<glm::vec3*>(&NormalsBuffer.data[NormalsBufferView.byteOffset + NormalsAccessor.byteOffset]);
				}

				glm::vec2* VertexUVs = nullptr;
				if (HasTextureCoords)
				{
					tinygltf::Accessor& TexCoordsAccessor = ModelData.accessors[Primitive.attributes.at("TEXCOORD_0")];
					tinygltf::BufferView& TexCoordsBufferView = ModelData.bufferViews[TexCoordsAccessor.bufferView];
					tinygltf::Buffer& TexCoordsBuffer = ModelData.buffers[TexCoordsBufferView.buffer];
					VertexUVs = reinterpret_cast<glm::vec2*>(&TexCoordsBuffer.data[TexCoordsBufferView.byteOffset + TexCoordsAccessor.byteOffset]);
				}

				OutData.Vertices.resize(PositionsAccessor.count);
				for (size_t VertexIndex = 0; VertexIndex < PositionsAccessor.count; VertexIndex++)
				{
					Vertex& VertexData = OutData.Vertices[VertexIndex];
					VertexData.Position = VertexPositions[VertexIndex];
					VertexData.Normal = VertexNormals != nullptr ? VertexNormals[VertexIndex] : glm::vec3{ 0.0f, 0.0f, 0.0f };
					VertexData.TextureCoordinate = VertexUVs != nullptr ? VertexUVs[VertexIndex] : glm::vec2{ 0.0f, 0.0f };
				}

				// Indices
				const tinygltf::Accessor& IndicesAccessor = ModelData.accessors[Primitive.indices];
				const tinygltf::BufferView& IndicesBufferView = ModelData.bufferViews[IndicesAccessor.bufferView];
				const tinygltf::Buffer& IndicesBuffer = ModelData.buffers[IndicesBufferView.buffer];
				const uint16_t* IndicesData = reinterpret_cast<const uint16_t*>(&IndicesBuffer.data[IndicesBufferView.byteOffset + IndicesAccessor.byteOffset]);

				OutData.Indices.resize(IndicesAccessor.count / 3);
				for (size_t i = 0; i < IndicesAccessor.count / 3; i++)
				{
					Index& IndexData = OutData.Indices[i];
					IndexData.Vertex0 = IndicesData[i * 3 + 0];
					IndexData.Vertex1 = IndicesData[i * 3 + 1];
					IndexData.Vertex2 = IndicesData[i * 3 + 2];
				}
			}

			// NOTE(Peter): We should be creating submeshes here, but for now we'll only process the first submesh
			return true;
		}

		return false;
	}

}
