#pragma once

//#include "VertexBuffer.hpp"
//#include "IndexBuffer.hpp"

#include "Buffer.hpp"

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

		Shared<Buffer> GetVertexBuffer() const { return m_VertexBuffer; }
		Shared<Buffer> GetIndexBuffer() const { return m_IndexBuffer; }

	private:
		std::vector<Vertex> m_Vertices;
		std::vector<Index> m_Indices;

		Shared<Buffer> m_VertexBuffer = nullptr;
		Shared<Buffer> m_IndexBuffer = nullptr;
	};

}
