#include "FusionPCH.hpp"
#include "D3D11IndexBuffer.hpp"
#include "D3D11Context.hpp"

namespace Fusion {

	D3D11IndexBuffer::D3D11IndexBuffer(uint32_t InSize)
		: m_Size(InSize)
	{
		Shared<D3D11Context> Context = GraphicsContext::Get<D3D11Context>();

		D3D11_BUFFER_DESC BufferDesc;
		ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
		BufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		BufferDesc.ByteWidth = InSize;
		BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		BufferDesc.CPUAccessFlags = 0;

		Context->GetDevice()->CreateBuffer(&BufferDesc, nullptr, &m_Buffer);
	}

	D3D11IndexBuffer::D3D11IndexBuffer(void* InData, uint32_t InSize)
		: m_Size(InSize)
	{
		Shared<D3D11Context> Context = GraphicsContext::Get<D3D11Context>();

		D3D11_BUFFER_DESC BufferDesc;
		ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
		BufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		BufferDesc.ByteWidth = InSize;
		BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		BufferDesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitialData;
		ZeroMemory(&InitialData, sizeof(D3D11_SUBRESOURCE_DATA));
		InitialData.pSysMem = InData;
		InitialData.SysMemPitch = 0;
		InitialData.SysMemSlicePitch = 0;

		Context->GetDevice()->CreateBuffer(&BufferDesc, &InitialData, &m_Buffer);
	}

	D3D11IndexBuffer::~D3D11IndexBuffer()
	{
		FUSION_RELEASE_COM(m_Buffer);
	}

	void D3D11IndexBuffer::Bind() const
	{
		GraphicsContext::Get<D3D11Context>()->GetDeviceContext()->IASetIndexBuffer(m_Buffer, DXGI_FORMAT_R32_UINT, 0);
	}

}
