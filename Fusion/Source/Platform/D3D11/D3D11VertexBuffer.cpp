#include "FusionPCH.h"
#include "D3D11VertexBuffer.h"
#include "D3D11Context.h"

namespace Fusion {

	static D3D11_USAGE BufferUsageToD3D11Usage(EBufferUsage InUsage)
	{
		switch (InUsage)
		{
		case EBufferUsage::Static: return D3D11_USAGE_DEFAULT;
		case EBufferUsage::Dynamic: return D3D11_USAGE_DYNAMIC;
		case EBufferUsage::Immutable: return D3D11_USAGE_IMMUTABLE;
		}

		FUSION_CORE_VERIFY(false);
		return D3D11_USAGE_DEFAULT;
	}

	D3D11VertexBuffer::D3D11VertexBuffer(const VertexBufferInfo& InCreateInfo)
		: m_Layout(InCreateInfo.Layout), m_Usage(InCreateInfo.Usage), m_Size(InCreateInfo.BufferSize)
	{
		Shared<D3D11Context> Context = GraphicsContext::Get<D3D11Context>();

		D3D11_BUFFER_DESC BufferDesc;
		ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
		BufferDesc.Usage = BufferUsageToD3D11Usage(InCreateInfo.Usage);
		BufferDesc.ByteWidth = InCreateInfo.BufferSize;
		BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA InitialData;
		ZeroMemory(&InitialData, sizeof(D3D11_SUBRESOURCE_DATA));
		InitialData.pSysMem = InCreateInfo.Data;
		InitialData.SysMemPitch = 0;
		InitialData.SysMemSlicePitch = 0;

		Context->GetDevice()->CreateBuffer(&BufferDesc, &InitialData, &m_Buffer);
	}

	D3D11VertexBuffer::~D3D11VertexBuffer()
	{

	}

	void D3D11VertexBuffer::Resize(uint32_t InNewSize)
	{
	}

	void D3D11VertexBuffer::SetData(void* InData, uint32_t InSize)
	{
		Shared<D3D11Context> Context = GraphicsContext::Get<D3D11Context>();

		D3D11_MAPPED_SUBRESOURCE MappedSubResource;
		ZeroMemory(&MappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

		Context->GetDeviceContext()->Map(m_Buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &MappedSubResource);
		memcpy(MappedSubResource.pData, InData, InSize);
		Context->GetDeviceContext()->Unmap(m_Buffer, NULL);
	}

}
