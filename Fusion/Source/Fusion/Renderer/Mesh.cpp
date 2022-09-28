#include "FusionPCH.h"
#include "Mesh.h"

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
#include <TinyGLTF/tiny_gltf.h>

namespace Fusion {

	Shared<Mesh> MeshLoader::LoadMeshFromFile(const std::filesystem::path& InFilePath)
	{
		tinygltf::TinyGLTF Loader;
		tinygltf::Model ModelData;
		std::string WarningMessage, ErrorMessage;

		const bool IsBinaryFile = InFilePath.extension().string().find(".glb") != std::string::npos;

		bool Loaded;

		if (IsBinaryFile)
			Loaded = Loader.LoadBinaryFromFile(&ModelData, &ErrorMessage, &WarningMessage, InFilePath.string());
		else
			Loaded = Loader.LoadASCIIFromFile(&ModelData, &ErrorMessage, &WarningMessage, InFilePath.string());

		if (!ErrorMessage.empty())
		{
			FUSION_CORE_ERROR("Error generated while loading mesh '{}'!", InFilePath.string());
			FUSION_CORE_ERROR("Error: {}", ErrorMessage);
			return nullptr;
		}

		if (!WarningMessage.empty())
		{
			FUSION_CORE_WARN("Warning generated while loading mesh '{}'!", InFilePath.string());
			FUSION_CORE_WARN("Warning: {}", WarningMessage);
		}

		if (!Loaded)
		{
			FUSION_CORE_ERROR("Failed to load glTF file '{}'!", InFilePath.string());
			return nullptr;
		}

		for (const tinygltf::Mesh& MeshData : ModelData.meshes)
		{
			// NOTE(Peter): We should be creating submeshes here, but for now we'll only process the first submesh

			std::vector<Vertex> Vertices;
			std::vector<Index> Indices;

			for (const tinygltf::Primitive& Primitive : MeshData.primitives)
			{
				// NOTE(Peter): We really shouldn't assume that *every* mesh has normals or texture coords
				const bool HasNormals = Primitive.attributes.find("NORMAL") != Primitive.attributes.end();
				const bool HasTextureCoords = Primitive.attributes.find("TEXCOORD_0") != Primitive.attributes.end();
				
				const tinygltf::Accessor& PositionsAccessor = ModelData.accessors[Primitive.attributes.at("POSITION")];
				const tinygltf::BufferView& PositionsBufferView = ModelData.bufferViews[PositionsAccessor.bufferView];
				const tinygltf::Buffer& PositionsBuffer = ModelData.buffers[PositionsBufferView.buffer];
				const glm::vec3* VertexPositions = reinterpret_cast<const glm::vec3*>(&PositionsBuffer.data[PositionsBufferView.byteOffset + PositionsAccessor.byteOffset]);

				const tinygltf::Accessor& NormalsAccessor = ModelData.accessors[Primitive.attributes.at("NORMAL")];
				const tinygltf::BufferView& NormalsBufferView = ModelData.bufferViews[NormalsAccessor.bufferView];
				const tinygltf::Buffer& NormalsBuffer = ModelData.buffers[NormalsBufferView.buffer];
				const glm::vec3* VertexNormals = reinterpret_cast<const glm::vec3*>(&NormalsBuffer.data[NormalsBufferView.byteOffset + NormalsAccessor.byteOffset]);

				const tinygltf::Accessor& TexCoordsAccessor = ModelData.accessors[Primitive.attributes.at("TEXCOORD_0")];
				const tinygltf::BufferView& TexCoordsBufferView = ModelData.bufferViews[TexCoordsAccessor.bufferView];
				const tinygltf::Buffer& TexCoordsBuffer = ModelData.buffers[TexCoordsBufferView.buffer];
				const glm::vec2* VertexUVs = reinterpret_cast<const glm::vec2*>(&TexCoordsBuffer.data[TexCoordsBufferView.byteOffset + TexCoordsAccessor.byteOffset]);

				for (size_t VertexIndex = 0; VertexIndex < PositionsAccessor.count; VertexIndex++)
				{
					Vertex VertexData;
					VertexData.Position = VertexPositions[VertexIndex];
					VertexData.Normal = VertexNormals[VertexIndex];
					VertexData.TextureCoordinate = VertexUVs[VertexIndex];
					Vertices.push_back(VertexData);
				}

				// Indices
				const tinygltf::Accessor& IndicesAccessor = ModelData.accessors[Primitive.indices];
				const tinygltf::BufferView& IndicesBufferView = ModelData.bufferViews[IndicesAccessor.bufferView];
				const tinygltf::Buffer& IndicesBuffer = ModelData.buffers[IndicesBufferView.buffer];
				const uint16_t* IndicesData = reinterpret_cast<const uint16_t*>(&IndicesBuffer.data[IndicesBufferView.byteOffset + IndicesAccessor.byteOffset]);
				for (size_t i = 0; i < IndicesAccessor.count / 3; i++)
					Indices.push_back({ IndicesData[i * 3 + 0], IndicesData[i * 3 + 1], IndicesData[i * 3 + 2] });
			}

			return Shared<Mesh>::Create(Vertices, Indices);
		}

		return nullptr;
	}

	Mesh::Mesh(const std::vector<Vertex>& InVertices, const std::vector<Index>& InIndices)
		: m_Vertices(InVertices), m_Indices(InIndices)
	{
		VertexBufferLayout Layout = {
			{ 0, ShaderDataType::Float3, offsetof(Vertex, Position) },
			{ 1, ShaderDataType::Float3, offsetof(Vertex, Normal) },
			{ 2, ShaderDataType::Float2, offsetof(Vertex, TextureCoordinate) }
		};

		m_VertexBuffer = VertexBuffer::Create(m_Vertices.size() * sizeof(Vertex), m_Vertices.data(), Layout);
		m_IndexBuffer = IndexBuffer::Create(m_Indices.size() * 3 * sizeof(uint32_t), m_Indices.data());
	}

	Mesh::~Mesh()
	{

	}

}