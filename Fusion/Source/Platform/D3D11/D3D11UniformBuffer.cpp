#include "FusionPCH.h"
#include "D3D11UniformBuffer.h"
#include "D3D11Context.h"
#include "D3D11Common.h"

namespace Fusion {

	D3D11UniformBuffer::D3D11UniformBuffer(uint32_t InSize, EShaderBindPoint InBindPoint, EBufferUsage InUsage)
		: m_Size(InSize), m_BindPoint(InBindPoint)
	{
		Shared<D3D11Context> Context = GraphicsContext::Get<D3D11Context>();

		D3D11_BUFFER_DESC BufferDesc;
		ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
		BufferDesc.Usage = BufferUsageToD3D11Usage(InUsage);
		BufferDesc.ByteWidth = InSize;
		BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		Context->GetDevice()->CreateBuffer(&BufferDesc, nullptr, &m_Buffer);
	}

	D3D11UniformBuffer::D3D11UniformBuffer(void* InData, uint32_t InSize, EShaderBindPoint InBindPoint, EBufferUsage InUsage)
		: m_Size(InSize), m_BindPoint(InBindPoint)
	{
		Shared<D3D11Context> Context = GraphicsContext::Get<D3D11Context>();

		D3D11_BUFFER_DESC BufferDesc;
		ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
		BufferDesc.Usage = BufferUsageToD3D11Usage(InUsage);
		BufferDesc.ByteWidth = InSize;
		BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA InitialData;
		ZeroMemory(&InitialData, sizeof(D3D11_SUBRESOURCE_DATA));
		InitialData.pSysMem = InData;
		InitialData.SysMemPitch = 0;
		InitialData.SysMemSlicePitch = 0;

		Context->GetDevice()->CreateBuffer(&BufferDesc, &InitialData, &m_Buffer);
	}

	D3D11UniformBuffer::~D3D11UniformBuffer()
	{
		FUSION_RELEASE_COM(m_Buffer);
	}

	void D3D11UniformBuffer::SetData(void* InData)
	{
		Shared<D3D11Context> Context = GraphicsContext::Get<D3D11Context>();

		D3D11_MAPPED_SUBRESOURCE MappedSubResource;
		ZeroMemory(&MappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

		Context->GetDeviceContext()->Map(m_Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);
		memcpy(MappedSubResource.pData, InData, m_Size);
		Context->GetDeviceContext()->Unmap(m_Buffer, 0);
	}

}
