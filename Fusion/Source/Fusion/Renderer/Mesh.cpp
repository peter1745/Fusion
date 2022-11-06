#include "FusionPCH.hpp"
#include "Mesh.hpp"

namespace Fusion {

	Mesh::Mesh(const std::vector<Vertex>& InVertices, const std::vector<Index>& InIndices)
		: m_Vertices(InVertices), m_Indices(InIndices)
	{
		VertexBufferInfo CreateInfo;
		CreateInfo.BufferSize = uint32_t(m_Vertices.size()) * sizeof(Vertex);
		CreateInfo.Stride = sizeof(Vertex);
		CreateInfo.Data = static_cast<void*>(m_Vertices.data());
		CreateInfo.Usage = EBufferUsage::Immutable;
		m_VertexBuffer = VertexBuffer::Create(CreateInfo);
		m_IndexBuffer = IndexBuffer::Create(uint32_t(m_Indices.size()) * 3 * sizeof(uint32_t), m_Indices.data());
	}

	Mesh::~Mesh()
	{

	}

}