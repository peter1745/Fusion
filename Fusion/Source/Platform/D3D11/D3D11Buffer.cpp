#include "FusionPCH.hpp"
#include "D3D11Buffer.hpp"
#include "D3D11Context.hpp"

#include "Fusion/Memory/Utils.h"

namespace Fusion {

	static constexpr UINT BufferStateToD3D11BindFlags(EBufferState InState)
	{
		UINT Result = 0;

		if (InState & BufferStates::Common) Result |= 0;
		if (InState & BufferStates::Vertex) Result |= D3D11_BIND_VERTEX_BUFFER;
		if (InState & BufferStates::Constant) Result |= D3D11_BIND_CONSTANT_BUFFER;
		if (InState & BufferStates::Index) Result |= D3D11_BIND_INDEX_BUFFER;
		if (InState & BufferStates::UnorderedAccess) Result |= D3D11_BIND_UNORDERED_ACCESS;

		return Result;
	}

	static constexpr D3D11_USAGE HeapTypeToD3D11Usage(EHeapType InHeapType)
	{
		switch (InHeapType)
		{
		case EHeapType::Default: return D3D11_USAGE_DEFAULT;
		case EHeapType::Upload: return D3D11_USAGE_DYNAMIC;
		case EHeapType::Readback: return D3D11_USAGE_STAGING;
		}

		return D3D11_USAGE_DEFAULT;
	}

	D3D11Buffer::D3D11Buffer(const BufferInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo)
	{
		auto& Device = GraphicsContext::Get<D3D11Context>()->GetDevice().As<D3D11Device>()->GetDevice();

		D3D11_BUFFER_DESC BufferDesc = {};
		BufferDesc.Usage = HeapTypeToD3D11Usage(InCreateInfo.HeapType);
		BufferDesc.ByteWidth = Align<16>(InCreateInfo.Size);
		BufferDesc.BindFlags = BufferStateToD3D11BindFlags(InCreateInfo.State);

		switch (InCreateInfo.HeapType)
		{
		case EHeapType::Default:
			BufferDesc.CPUAccessFlags = 0;
			break;
		case EHeapType::Upload:
			BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			break;
		case EHeapType::Readback:
			BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
			break;
		}

		D3D11_SUBRESOURCE_DATA InitialData = {};

		if (InCreateInfo.InitialData != nullptr)
		{
			InitialData.pSysMem = InCreateInfo.InitialData;
			InitialData.SysMemPitch = 0;
			InitialData.SysMemSlicePitch = 0;
		}

		Device->CreateBuffer(&BufferDesc, InCreateInfo.InitialData == nullptr ? nullptr : &InitialData, m_Buffer);
	}

	D3D11Buffer::~D3D11Buffer() {}

	Byte* D3D11Buffer::Map()
	{
		D3D11_MAPPED_SUBRESOURCE MappedResource = {};
		GraphicsContext::Get<D3D11Context>()->GetDevice().As<D3D11Device>()->GetDeviceContext()->Map(m_Buffer, 0, D3D11_MAP_READ, 0, &MappedResource);
		return reinterpret_cast<Byte*>(MappedResource.pData);
	}

	void D3D11Buffer::Unmap(Byte* InPtr)
	{
		GraphicsContext::Get<D3D11Context>()->GetDevice().As<D3D11Device>()->GetDeviceContext()->Unmap(m_Buffer, 0);
	}

	void D3D11Buffer::Transition(CommandList* InCmdList, EBufferState InState) {}

	void D3D11Buffer::SetData(CommandList* InCmdList, const void* InData, const Shared<Buffer>& InUploadBuffer)
	{
		auto& DeviceContext = GraphicsContext::Get<D3D11Context>()->GetDevice().As<D3D11Device>()->GetDeviceContext();
		auto UploadBuffer = InUploadBuffer.As<D3D11Buffer>();

		D3D11_MAPPED_SUBRESOURCE MappedResource = {};
		DeviceContext->Map(UploadBuffer->GetResource(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
		memcpy(MappedResource.pData, InData, m_CreateInfo.Size);
		DeviceContext->Unmap(UploadBuffer->GetResource(), 0);

		DeviceContext->CopyResource(m_Buffer, UploadBuffer->GetResource());
	}

}
