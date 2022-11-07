#include "FusionPCH.hpp"
#include "Mesh.hpp"

namespace Fusion {

	Mesh::Mesh(const std::vector<Vertex>& InVertices, const std::vector<Index>& InIndices)
		: m_Vertices(InVertices), m_Indices(InIndices)
	{
		BufferInfo VertexBufferInfo = {};
		VertexBufferInfo.HeapType = EHeapType::Default;
		VertexBufferInfo.State = BufferStates::Vertex;
		VertexBufferInfo.Size = InVertices.size() * sizeof(Vertex);
		VertexBufferInfo.InitialData = m_Vertices.data();
		m_VertexBuffer = Buffer::Create(VertexBufferInfo);

		BufferInfo IndexBufferInfo = {};
		IndexBufferInfo.HeapType = EHeapType::Default;
		IndexBufferInfo.State = BufferStates::Index;
		IndexBufferInfo.Size = InIndices.size() * sizeof(Index);
		IndexBufferInfo.InitialData = m_Indices.data();
		m_IndexBuffer = Buffer::Create(IndexBufferInfo);
	}

	Mesh::~Mesh()
	{

	}

}