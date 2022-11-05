#include "FusionPCH.hpp"
#include "D3D12StagingBuffer.hpp"
#include "D3D12Context.hpp"
#include "D3D12Image2D.hpp"

#include "Fusion/Memory/Utils.h"

namespace Fusion {

	D3D12StagingBuffer::D3D12StagingBuffer(const StagingBufferInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo)
	{
		D3D12_HEAP_PROPERTIES HeapProperties = {};
		HeapProperties.Type = D3D12_HEAP_TYPE_READBACK;
		HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		HeapProperties.CreationNodeMask = 0;
		HeapProperties.VisibleNodeMask = 0;

		D3D12_RESOURCE_DESC1 BufferDesc = {};
		BufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		BufferDesc.Alignment = 0;
		BufferDesc.Width = InCreateInfo.Size;
		BufferDesc.Height = 1;
		BufferDesc.DepthOrArraySize = 1;
		BufferDesc.MipLevels = 1;
		BufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		BufferDesc.SampleDesc.Count = 1;
		BufferDesc.SampleDesc.Quality = 0;
		BufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		BufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		BufferDesc.SamplerFeedbackMipRegion.Width = 0;
		BufferDesc.SamplerFeedbackMipRegion.Height = 0;
		BufferDesc.SamplerFeedbackMipRegion.Depth = 0;

		Fusion::GraphicsContext::Get<Fusion::D3D12Context>()->GetDevice()->CreateCommittedResource2(&HeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&BufferDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			nullptr,
			m_Buffer, m_Buffer
		);
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
		DstLocation.pResource = m_Buffer;
		DstLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		DstLocation.PlacedFootprint.Offset = 0;
		DstLocation.PlacedFootprint.Footprint.Format = ImageFormatToDXGIFormat(ImageInfo.Format);
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

	Byte* D3D12StagingBuffer::Map()
	{
		Byte* BufferStart;
		D3D12_RANGE ReadRange = { 0, 0 };
		m_Buffer->Map(0, &ReadRange, reinterpret_cast<void**>(&BufferStart));
		return BufferStart;
	}

	void D3D12StagingBuffer::Unmap(Byte* InPtr)
	{
		m_Buffer->Unmap(0, nullptr);
	}

}
