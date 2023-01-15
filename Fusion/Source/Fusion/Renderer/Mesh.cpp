#include "FusionPCH.hpp"
#include "Mesh.hpp"

namespace Fusion {

	Mesh::Mesh(const std::vector<Vertex>& InVertices, const std::vector<Index>& InIndices)
		: m_Vertices(InVertices), m_Indices(InIndices)
	{
		BufferInfo VertexBufferInfo = {};
		VertexBufferInfo.Usage = EBufferUsage::Vertex;
		VertexBufferInfo.Size = InVertices.size() * sizeof(Vertex);
		VertexBufferInfo.InitialData = m_Vertices.data();
		m_VertexBuffer = Shared<Buffer>::Create(VertexBufferInfo);

		BufferInfo IndexBufferInfo = {};
		IndexBufferInfo.Usage = EBufferUsage::Index;
		IndexBufferInfo.Size = InIndices.size() * sizeof(Index);
		IndexBufferInfo.InitialData = m_Indices.data();
		m_IndexBuffer = Shared<Buffer>::Create(IndexBufferInfo);
	}

	Mesh::~Mesh()
	{

	}

}