#include "FusionPCH.hpp"
#include "D3D12StagingBuffer.hpp"
#include "D3D12Context.hpp"
#include "D3D12Image2D.hpp"

#include "Fusion/Memory/Utils.h"

namespace Fusion {

	D3D12StagingBuffer::D3D12StagingBuffer(const StagingBufferInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo)
	{
		BufferInfo BufferCreateInfo = {};
		BufferCreateInfo.HeapType = EHeapType::Readback;
		BufferCreateInfo.State = BufferStates::CopyDestination;
		BufferCreateInfo.Size = InCreateInfo.Size;
		m_Buffer = MakeUnique<D3D12Buffer>(BufferCreateInfo);
	}

	D3D12StagingBuffer::~D3D12StagingBuffer()
	{

	}

	void D3D12StagingBuffer::CopyFrom(CommandList* InCmdList, const Shared<Image2D>& InImage, const CopyRegionInfo& InCopyRegion)
	{
		FUSION_CORE_VERIFY(InImage->GetState() & ImageStates::CopySrc, "Trying to copy from an image which isn't in a Copy Source state");

		const auto& ImageInfo = InImage->GetInfo();

		size_t FormatSize = GetFormatSize(ImageInfo.Format);

		D3D12_TEXTURE_COPY_LOCATION DstLocation = {};
		DstLocation.pResource = m_Buffer->GetBuffer();
		DstLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		DstLocation.PlacedFootprint.Offset = 0;
		DstLocation.PlacedFootprint.Footprint.Format = EFormatToDXGIFormat(ImageInfo.Format);
		DstLocation.PlacedFootprint.Footprint.Width = m_CreateInfo.Size / FormatSize;
		DstLocation.PlacedFootprint.Footprint.Height = 1;
		DstLocation.PlacedFootprint.Footprint.Depth = 1;
		DstLocation.PlacedFootprint.Footprint.RowPitch = Fusion::Align<256>(m_CreateInfo.Size);

		D3D12_TEXTURE_COPY_LOCATION SrcLocation = {};
		SrcLocation.pResource = InImage.As<Fusion::D3D12Image2D>()->GetResource();
		SrcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		SrcLocation.SubresourceIndex = 0;

		D3D12_BOX CopyRegion = {};
		CopyRegion.left = InCopyRegion.Left;
		CopyRegion.top = InCopyRegion.Top;
		CopyRegion.front = 0;
		CopyRegion.right = InCopyRegion.Right;
		CopyRegion.bottom = InCopyRegion.Bottom;
		CopyRegion.back = 1;

		static_cast<D3D12CommandList*>(InCmdList)->GetNativeList()->CopyTextureRegion(&DstLocation, 0, 0, 0, &SrcLocation, &CopyRegion);
	}

}