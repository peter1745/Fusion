#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include <filesystem>
#include <glm/glm.hpp>

namespace Fusion {

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TextureCoordinate;
	};

	struct Index
	{
		uint32_t Vertex0;
		uint32_t Vertex1;
		uint32_t Vertex2;
	};

	class Mesh : public SharedObject
	{
	public:
		Mesh(const std::vector<Vertex>& InVertices, const std::vector<Index>& InIndices);
		~Mesh();

		Shared<VertexBuffer> GetVertexBuffer() const { return m_VertexBuffer; }
		Shared<IndexBuffer> GetIndexBuffer() const { return m_IndexBuffer; }

	private:
		std::vector<Vertex> m_Vertices;
		std::vector<Index> m_Indices;

		Shared<VertexBuffer> m_VertexBuffer = nullptr;
		Shared<IndexBuffer> m_IndexBuffer = nullptr;
	};

	class MeshLoader
	{
	public:
		static Shared<Mesh> LoadMeshFromFile(const std::filesystem::path& InFilePath);
	};

}
