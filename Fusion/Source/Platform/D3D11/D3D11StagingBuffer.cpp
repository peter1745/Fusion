#include "FusionPCH.hpp"
#include "D3D11StagingBuffer.hpp"
#include "D3D11Context.hpp"
#include "D3D11Image.hpp"

namespace Fusion {

	D3D11StagingBuffer::D3D11StagingBuffer(const StagingBufferInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo)
	{
		/*D3D11_BUFFER_DESC StagingBufferDesc = {};
		StagingBufferDesc.ByteWidth = InCreateInfo.Size;
		StagingBufferDesc.Usage = D3D11_USAGE_STAGING;
		StagingBufferDesc.BindFlags = 0;
		StagingBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		StagingBufferDesc.MiscFlags = 0;
		StagingBufferDesc.StructureByteStride = GetFormatSize(InCreateInfo.Format);
		Fusion::GraphicsContext::Get<Fusion::D3D11Context>()->GetDevice()->CreateBuffer(&StagingBufferDesc, nullptr, m_Buffer);*/

		D3D11_TEXTURE2D_DESC StagingTextureDesc = {};
		StagingTextureDesc.Width = InCreateInfo.Size / GetFormatSize(InCreateInfo.Format);
		StagingTextureDesc.Height = 1;
		StagingTextureDesc.MipLevels = 1;
		StagingTextureDesc.ArraySize = 1;
		StagingTextureDesc.Format = EFormatToDXGIFormat(InCreateInfo.Format);
		StagingTextureDesc.SampleDesc.Count = 1;
		StagingTextureDesc.Usage = D3D11_USAGE_STAGING;
		StagingTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		StagingTextureDesc.MiscFlags = 0;
		GraphicsContext::Get<D3D11Context>()->GetDevice()->CreateTexture2D(&StagingTextureDesc, nullptr, m_Buffer);
	}

	D3D11StagingBuffer::~D3D11StagingBuffer()
	{

	}

	void D3D11StagingBuffer::CopyFrom(CommandList* InCmdList, const Shared<Image2D>& InImage, const CopyRegionInfo& InCopyRegion)
	{
		auto& CmdList = static_cast<D3D11CommandList*>(InCmdList)->GetNativeList();

		D3D11_BOX CopyRegion = {};
		CopyRegion.left = InCopyRegion.Left;
		CopyRegion.top = InCopyRegion.Top;
		CopyRegion.right = InCopyRegion.Right;
		CopyRegion.bottom = InCopyRegion.Bottom;
		CopyRegion.front = 0;
		CopyRegion.back = 1;
		CmdList->CopySubresourceRegion(m_Buffer, 0, 0, 0, 0, InImage.As<D3D11Image2D>()->GetResource(), 0, &CopyRegion);
	}

	Byte* D3D11StagingBuffer::Map()
	{
		auto& DeviceContext = GraphicsContext::Get<D3D11Context>()->GetDeviceContext();
		D3D11_MAPPED_SUBRESOURCE MappedResource = {};
		DeviceContext->Map(m_Buffer, 0, D3D11_MAP_READ, 0, &MappedResource);
		return static_cast<Byte*>(MappedResource.pData);
	}

	void D3D11StagingBuffer::Unmap(Byte* InPtr)
	{
		auto& DeviceContext = GraphicsContext::Get<D3D11Context>()->GetDeviceContext();
		DeviceContext->Unmap(m_Buffer, 0);
	}

}