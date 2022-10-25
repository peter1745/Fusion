#include "FusionPCH.hpp"
#include "D3D11VertexBuffer.hpp"
#include "D3D11Context.hpp"
#include "D3D11Common.hpp"

namespace Fusion {

	D3D11VertexBuffer::D3D11VertexBuffer(const VertexBufferInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo)
	{
		Shared<D3D11Context> Context = GraphicsContext::Get<D3D11Context>();

		D3D11_BUFFER_DESC BufferDesc;
		ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
		BufferDesc.Usage = BufferUsageToD3D11Usage(InCreateInfo.Usage);
		BufferDesc.ByteWidth = InCreateInfo.BufferSize;
		BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		BufferDesc.CPUAccessFlags = InCreateInfo.Usage != EBufferUsage::Immutable ? D3D11_CPU_ACCESS_WRITE : 0;

		D3D11_SUBRESOURCE_DATA InitialData;
		ZeroMemory(&InitialData, sizeof(D3D11_SUBRESOURCE_DATA));
		InitialData.pSysMem = InCreateInfo.Data;
		InitialData.SysMemPitch = 0;
		InitialData.SysMemSlicePitch = 0;

		Context->GetDevice()->CreateBuffer(&BufferDesc, &InitialData, &m_Buffer);
	}

	D3D11VertexBuffer::~D3D11VertexBuffer()
	{
		FUSION_RELEASE_COM(m_Buffer);
	}

	void D3D11VertexBuffer::Bind() const
	{
		uint32_t Stride = m_CreateInfo.Layout.GetStride();
		uint32_t Offset = 0;
		GraphicsContext::Get<D3D11Context>()->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_Buffer, &Stride, &Offset);
	}

	void D3D11VertexBuffer::Resize(uint32_t InNewSize)
	{
		if (m_CreateInfo.Usage == EBufferUsage::Immutable)
			return;

		FUSION_RELEASE_COM(m_Buffer);

		m_CreateInfo.BufferSize = InNewSize;

		D3D11_BUFFER_DESC BufferDesc;
		ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
		BufferDesc.Usage = BufferUsageToD3D11Usage(m_CreateInfo.Usage);
		BufferDesc.ByteWidth = InNewSize;
		BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		GraphicsContext::Get<D3D11Context>()->GetDevice()->CreateBuffer(&BufferDesc, nullptr, &m_Buffer);
	}

	void D3D11VertexBuffer::SetData(void* InData, uint32_t InSize)
	{
		if (InSize > m_CreateInfo.BufferSize)
		{
			FUSION_CORE_VERIFY(m_CreateInfo.Usage != EBufferUsage::Immutable, "Cannot change size of immutable buffer!");
			Resize(InSize);
		}

		Shared<D3D11Context> Context = GraphicsContext::Get<D3D11Context>();

		D3D11_MAPPED_SUBRESOURCE MappedSubResource;
		ZeroMemory(&MappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

		Context->GetDeviceContext()->Map(m_Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);
		memcpy(MappedSubResource.pData, InData, InSize);
		Context->GetDeviceContext()->Unmap(m_Buffer, 0);
	}

}
