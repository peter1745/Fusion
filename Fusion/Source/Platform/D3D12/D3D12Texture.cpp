#include "FusionPCH.hpp"
#include "D3D12Texture.hpp"
#include "D3D12Context.hpp"

#include "Fusion/Memory/Utils.h"

namespace Fusion {

	D3D12Texture2D::D3D12Texture2D(const Texture2DInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo)
	{
		auto Context = GraphicsContext::Get<D3D12Context>();

		Image2DInfo ImageInfo = {};
		ImageInfo.Size = { InCreateInfo.Width, InCreateInfo.Height };
		ImageInfo.Usage = EImageUsage::Texture;
		ImageInfo.Format = InCreateInfo.Format;
		ImageInfo.Flags = 0;
		ImageInfo.InitialState = ImageStates::CopyDst;
		m_Image = Shared<D3D12Image2D>::Create(ImageInfo);

		BufferInfo UploadBufferInfo = {};
		UploadBufferInfo.HeapType = EHeapType::Upload;
		UploadBufferInfo.Size = InCreateInfo.Width * InCreateInfo.Height * GetFormatSize(InCreateInfo.Format);
		m_UploadBuffer = MakeUnique<D3D12Buffer>(UploadBufferInfo);

		Byte* BufferStart = m_UploadBuffer->Map();
		memcpy(BufferStart, InCreateInfo.Data, m_UploadBuffer->GetSize());
		m_UploadBuffer->Unmap(BufferStart);

		Shared<D3D12Texture2D> Instance = this;
		Context->SubmitToCommandContext([Instance](CommandList* InCmdList) mutable
		{
			Shared<D3D12Image2D> Image = Instance->m_Image;
			const auto& ImageInfo = Image->GetInfo();
			size_t FormatSize = GetFormatSize(ImageInfo.Format);

			D3D12_TEXTURE_COPY_LOCATION SrcLocation = {};
			SrcLocation.pResource = Instance->m_UploadBuffer->GetBuffer();
			SrcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			SrcLocation.PlacedFootprint.Offset = 0;
			SrcLocation.PlacedFootprint.Footprint.Format = EFormatToDXGIFormat(ImageInfo.Format);
			SrcLocation.PlacedFootprint.Footprint.Width = Instance->m_CreateInfo.Width;
			SrcLocation.PlacedFootprint.Footprint.Height = Instance->m_CreateInfo.Height;
			SrcLocation.PlacedFootprint.Footprint.Depth = 1;
			SrcLocation.PlacedFootprint.Footprint.RowPitch = Align<256>(Instance->m_CreateInfo.Width * GetFormatSize(ImageInfo.Format));

			D3D12_TEXTURE_COPY_LOCATION DstLocation = {};
			DstLocation.pResource = Image->GetResource();
			DstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			DstLocation.SubresourceIndex = 0;

			auto& CmdList = static_cast<D3D12CommandList*>(InCmdList)->GetNativeList();
			CmdList->CopyTextureRegion(&DstLocation, 0, 0, 0, &SrcLocation, nullptr);

			Image->Transition(InCmdList, ImageStates::PixelShaderResource);

			auto Heap = GraphicsContext::Get<D3D12Context>()->GetDescriptorHeap(EDescriptorHeapType::SRV_CBV_UAV);
			Instance->m_ShaderResourceViewAllocation = Heap->AllocateShaderResourceView(Instance);
		});
	}

	void D3D12Texture2D::Bind(uint32_t InSlot)
	{
		/*if (m_ShaderResourceViewAllocation.Heap != nullptr && m_Image->GetState() & ImageStates::PixelShaderResource)
		{
			auto& NativeList = static_cast<D3D12CommandList*>(GraphicsContext::Get<D3D12Context>()->GetCurrentCommandList())->GetNativeList();
			D3D12_GPU_DESCRIPTOR_HANDLE Handle = { GraphicsContext::Get<D3D12Context>()->GetDescriptorHeap(EDescriptorHeapType::SRV_CBV_UAV)->GetGPUDescriptorHandle(m_ShaderResourceViewAllocation) };
			NativeList->SetGraphicsRootDescriptorTable(InSlot, Handle);
		}*/
	}

}

