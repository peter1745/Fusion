#include "FusionPCH.hpp"
#include "D3D12RenderTexture.hpp"
#include "D3D12Context.hpp"

namespace Fusion {

	static D3D12_RESOURCE_FLAGS EImageFlagsToD3D12ResourceFlags(EImageFlag InFlags)
	{
		uint64_t Result = 0;

		if (InFlags & ImageFlags::AllowRenderTarget) Result |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		if (InFlags & ImageFlags::AllowDepthStencil) Result |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		return static_cast<D3D12_RESOURCE_FLAGS>(Result);
	}

	static D3D12_RESOURCE_STATES EImageStatesToD3D12ResourceStates(EImageState InStates)
	{
		uint64_t Result = 0;

		if (InStates & ImageStates::RenderTarget) Result |= D3D12_RESOURCE_STATE_RENDER_TARGET;
		if (InStates & ImageStates::DepthWrite) Result |= D3D12_RESOURCE_STATE_DEPTH_WRITE;
		if (InStates & ImageStates::DepthRead) Result |= D3D12_RESOURCE_STATE_DEPTH_READ;
		if (InStates & ImageStates::UnorderedAccess) Result |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		if (InStates & ImageStates::NonPixelShaderResource) Result |= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		if (InStates & ImageStates::PixelShaderResource) Result |= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		if (InStates & ImageStates::IndirectArgument) Result |= D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
		if (InStates & ImageStates::CopyDst) Result |= D3D12_RESOURCE_STATE_COPY_DEST;
		if (InStates & ImageStates::CopySrc) Result |= D3D12_RESOURCE_STATE_COPY_SOURCE;
		if (InStates & ImageStates::ResolveDst) Result |= D3D12_RESOURCE_STATE_RESOLVE_DEST;
		if (InStates & ImageStates::ResolveSrc) Result |= D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
		if (InStates & ImageStates::ShadingRateSrc) Result |= D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE;

		return static_cast<D3D12_RESOURCE_STATES>(Result);
	}

	D3D12RenderTexture::D3D12RenderTexture(const RenderTextureInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo)
	{
		auto Context = GraphicsContext::Get<D3D12Context>();
		auto& Device = Context->GetDevice();

		m_Attachments.resize(InCreateInfo.ColorAttachments.size());

		for (size_t Idx = 0; Idx < InCreateInfo.ColorAttachments.size(); Idx++)
		{
			const auto& AttachmentInfo = InCreateInfo.ColorAttachments[Idx];
			auto& Attachment = m_Attachments[Idx];

			Attachment.Images.resize(Context->GetFramesInFlight());
			Attachment.States.resize(Context->GetFramesInFlight(), AttachmentInfo.InitialState);
			Attachment.Sizes.resize(Context->GetFramesInFlight());

			D3D12_HEAP_PROPERTIES HeapProperties = {};
			HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
			HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			HeapProperties.CreationNodeMask = 0;
			HeapProperties.VisibleNodeMask = 0;

			D3D12_RESOURCE_DESC1 ResourceDesc = {};
			ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			ResourceDesc.Alignment = 0;
			ResourceDesc.Width = InCreateInfo.Width;
			ResourceDesc.Height = InCreateInfo.Height;
			ResourceDesc.DepthOrArraySize = 1;
			ResourceDesc.MipLevels = 1;
			ResourceDesc.Format = EGraphicsFormatToDXGIFormat(AttachmentInfo.Format);
			ResourceDesc.SampleDesc.Count = 1;
			ResourceDesc.SampleDesc.Quality = 0;
			ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			ResourceDesc.Flags = EImageFlagsToD3D12ResourceFlags(AttachmentInfo.Flags);

			ResourceDesc.SamplerFeedbackMipRegion.Width = 0;
			ResourceDesc.SamplerFeedbackMipRegion.Height = 0;
			ResourceDesc.SamplerFeedbackMipRegion.Depth = 0;

			D3D12_CLEAR_VALUE ClearValue = {};
			ClearValue.Format = ResourceDesc.Format;

			if (IsDepthFormat(AttachmentInfo.Format))
			{
				ClearValue.DepthStencil.Depth = 1.0f;
				ClearValue.DepthStencil.Stencil = 0.0f;
			}
			else
			{
				memcpy(ClearValue.Color, &AttachmentInfo.ClearColor[0], sizeof(AttachmentInfo.ClearColor));
			}

			for (size_t ImageIdx = 0; ImageIdx < Context->GetFramesInFlight(); ImageIdx++)
			{
				Device->CreateCommittedResource2(
					&HeapProperties,
					D3D12_HEAP_FLAG_NONE,
					&ResourceDesc,
					EImageStatesToD3D12ResourceStates(Attachment.States[ImageIdx] & ~(ImageStates::CopySrc | ImageStates::CopyDst)),
					&ClearValue,
					nullptr,
					Attachment.Images[ImageIdx], Attachment.Images[ImageIdx]
				);
			}
		}

		D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc = {};
		DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		DescriptorHeapDesc.NumDescriptors = Context->GetFramesInFlight() * m_Attachments.size();
		DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		DescriptorHeapDesc.NodeMask = 0;
		Device->CreateDescriptorHeap(&DescriptorHeapDesc, m_RenderTargetDescriptorHeap, m_RenderTargetDescriptorHeap);

		m_RenderTargetViewHandle = m_RenderTargetDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		m_RenderTargetViewHandleIncrementSize = Device->GetDescriptorHandleIncrementSize(DescriptorHeapDesc.Type);

		for (size_t Idx = 0; Idx < m_Attachments.size(); Idx++)
		{
			const auto& AttachmentInfo = InCreateInfo.ColorAttachments[Idx];
			auto& Attachment = m_Attachments[Idx];

			D3D12_RENDER_TARGET_VIEW_DESC ViewDesc = {};
			ViewDesc.Format = EGraphicsFormatToDXGIFormat(AttachmentInfo.Format);
			ViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

			ViewDesc.Texture2D.MipSlice = 0;
			ViewDesc.Texture2D.PlaneSlice = 0;

			for (size_t ImageIdx = 0; ImageIdx < Attachment.Images.size(); ImageIdx++)
			{
				D3D12_CPU_DESCRIPTOR_HANDLE Handle = m_RenderTargetViewHandle;
				Handle.ptr += (ImageIdx * m_Attachments.size() + Idx) * m_RenderTargetViewHandleIncrementSize;
				Device->CreateRenderTargetView(Attachment.Images[ImageIdx], &ViewDesc, Handle);
			}
		}

		// Depth Stencil Attachment
		{
			const auto& AttachmentInfo = InCreateInfo.DepthAttachment;

			m_DepthStencilAttachment.Images.resize(Context->GetFramesInFlight());
			m_DepthStencilAttachment.States.resize(Context->GetFramesInFlight(), AttachmentInfo.InitialState);
			m_DepthStencilAttachment.Sizes.resize(Context->GetFramesInFlight());

			D3D12_HEAP_PROPERTIES HeapProperties = {};
			HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
			HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			HeapProperties.CreationNodeMask = 0;
			HeapProperties.VisibleNodeMask = 0;

			D3D12_RESOURCE_DESC1 ResourceDesc = {};
			ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			ResourceDesc.Alignment = 0;
			ResourceDesc.Width = InCreateInfo.Width;
			ResourceDesc.Height = InCreateInfo.Height;
			ResourceDesc.DepthOrArraySize = 1;
			ResourceDesc.MipLevels = 1;
			ResourceDesc.Format = EGraphicsFormatToDXGIFormat(AttachmentInfo.Format);
			ResourceDesc.SampleDesc.Count = 1;
			ResourceDesc.SampleDesc.Quality = 0;
			ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			ResourceDesc.Flags = EImageFlagsToD3D12ResourceFlags(AttachmentInfo.Flags);

			ResourceDesc.SamplerFeedbackMipRegion.Width = 0;
			ResourceDesc.SamplerFeedbackMipRegion.Height = 0;
			ResourceDesc.SamplerFeedbackMipRegion.Depth = 0;

			D3D12_CLEAR_VALUE ClearValue = {};
			ClearValue.Format = ResourceDesc.Format;
			ClearValue.DepthStencil.Depth = 1.0f;
			ClearValue.DepthStencil.Stencil = 0.0f;

			for (size_t ImageIdx = 0; ImageIdx < Context->GetFramesInFlight(); ImageIdx++)
			{
				Device->CreateCommittedResource2(
					&HeapProperties,
					D3D12_HEAP_FLAG_NONE,
					&ResourceDesc,
					EImageStatesToD3D12ResourceStates(m_DepthStencilAttachment.States[ImageIdx] & ~(ImageStates::CopySrc | ImageStates::CopyDst)),
					&ClearValue,
					nullptr,
					m_DepthStencilAttachment.Images[ImageIdx], m_DepthStencilAttachment.Images[ImageIdx]
				);
			}
		}

		if (IsDepthFormat(InCreateInfo.DepthAttachment.Format))
		{
			DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			DescriptorHeapDesc.NumDescriptors = Context->GetFramesInFlight();
			Device->CreateDescriptorHeap(&DescriptorHeapDesc, m_DepthStencilDescriptorHeap, m_DepthStencilDescriptorHeap);

			m_DepthStencilViewHandle = m_DepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			m_DepthStencilViewHandleIncrementSize = Device->GetDescriptorHandleIncrementSize(DescriptorHeapDesc.Type);

			D3D12_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc = {};
			DepthStencilViewDesc.Format = EGraphicsFormatToDXGIFormat(InCreateInfo.DepthAttachment.Format);
			DepthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;
			DepthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			DepthStencilViewDesc.Texture2D.MipSlice = 0;

			for (size_t Idx = 0; Idx < Context->GetFramesInFlight(); Idx++)
			{
				D3D12_CPU_DESCRIPTOR_HANDLE Handle = m_DepthStencilViewHandle;
				Handle.ptr += Idx * m_DepthStencilViewHandleIncrementSize;
				Device->CreateDepthStencilView(m_DepthStencilAttachment.Images[Idx], &DepthStencilViewDesc, Handle);
			}
		}
	}

	void D3D12RenderTexture::Bind()
	{
		auto Context = GraphicsContext::Get<D3D12Context>();
		auto& Device = Context->GetDevice();
		auto& CmdList = static_cast<D3D12CommandList*>(Context->GetCurrentCommandList())->GetNativeList();
		uint32_t CurrentFrameIdx = Context->GetCurrentFrameIndex();

		// Set render targets + depth stencil (if available)
		{
			auto RTVStart = m_RenderTargetViewHandle;
			RTVStart.ptr += CurrentFrameIdx * m_Attachments.size() * m_RenderTargetViewHandleIncrementSize;

			auto DepthStencilStart = m_DepthStencilViewHandle;
			DepthStencilStart.ptr += CurrentFrameIdx * m_DepthStencilViewHandleIncrementSize;

			CmdList->OMSetRenderTargets(m_Attachments.size(), &RTVStart, true, m_DepthStencilDescriptorHeap.IsValid() ? &DepthStencilStart : nullptr);
		}

		// Clear render targets
		{
			for (size_t Idx = 0; Idx < m_Attachments.size(); Idx++)
			{
				auto Handle = m_RenderTargetViewHandle;
				Handle.ptr += (CurrentFrameIdx * m_Attachments.size() + Idx) * m_RenderTargetViewHandleIncrementSize;

				const auto& ClearColor = m_CreateInfo.ColorAttachments[Idx].ClearColor;

				CmdList->ClearRenderTargetView(Handle, &ClearColor[0], 0, nullptr);
			}
		}

		// Clear depth stencil
		if (m_DepthStencilDescriptorHeap.IsValid())
		{
			auto Handle = m_DepthStencilViewHandle;
			Handle.ptr += CurrentFrameIdx * m_DepthStencilViewHandleIncrementSize;
			CmdList->ClearDepthStencilView(Handle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		}
	}

	void D3D12RenderTexture::Unbind() {}
	void D3D12RenderTexture::Clear() {}

	void D3D12RenderTexture::Resize(uint32_t InWidth, uint32_t InHeight)
	{
		if (InWidth == 0 || InHeight == 0 || InWidth == m_CreateInfo.Width || InHeight == m_CreateInfo.Height)
			return;

		m_CreateInfo.Width = InWidth;
		m_CreateInfo.Height = InHeight;

		auto Context = GraphicsContext::Get<D3D12Context>();
		auto& Device = Context->GetDevice();

		for (size_t Idx = 0; Idx < m_CreateInfo.ColorAttachments.size(); Idx++)
		{
			const auto& AttachmentInfo = m_CreateInfo.ColorAttachments[Idx];
			auto& Attachment = m_Attachments[Idx];

			D3D12_HEAP_PROPERTIES HeapProperties = {};
			HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
			HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			HeapProperties.CreationNodeMask = 0;
			HeapProperties.VisibleNodeMask = 0;

			D3D12_RESOURCE_DESC1 ResourceDesc = {};
			ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			ResourceDesc.Alignment = 0;
			ResourceDesc.Width = m_CreateInfo.Width;
			ResourceDesc.Height = m_CreateInfo.Height;
			ResourceDesc.DepthOrArraySize = 1;
			ResourceDesc.MipLevels = 1;
			ResourceDesc.Format = EGraphicsFormatToDXGIFormat(AttachmentInfo.Format);
			ResourceDesc.SampleDesc.Count = 1;
			ResourceDesc.SampleDesc.Quality = 0;
			ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			ResourceDesc.Flags = EImageFlagsToD3D12ResourceFlags(AttachmentInfo.Flags);

			ResourceDesc.SamplerFeedbackMipRegion.Width = 0;
			ResourceDesc.SamplerFeedbackMipRegion.Height = 0;
			ResourceDesc.SamplerFeedbackMipRegion.Depth = 0;

			D3D12_CLEAR_VALUE ClearValue = {};
			ClearValue.Format = ResourceDesc.Format;

			if (IsDepthFormat(AttachmentInfo.Format))
			{
				ClearValue.DepthStencil.Depth = 1.0f;
				ClearValue.DepthStencil.Stencil = 0.0f;
			}
			else
			{
				memcpy(ClearValue.Color, &AttachmentInfo.ClearColor[0], sizeof(AttachmentInfo.ClearColor));
			}

			for (size_t ImageIdx = 0; ImageIdx < Context->GetFramesInFlight(); ImageIdx++)
			{
				Device->CreateCommittedResource2(
					&HeapProperties,
					D3D12_HEAP_FLAG_NONE,
					&ResourceDesc,
					EImageStatesToD3D12ResourceStates(Attachment.States[ImageIdx] & ~(ImageStates::CopySrc | ImageStates::CopyDst)),
					&ClearValue,
					nullptr,
					Attachment.Images[ImageIdx], Attachment.Images[ImageIdx]
				);
			}
		}

		for (size_t Idx = 0; Idx < m_Attachments.size(); Idx++)
		{
			const auto& AttachmentInfo = m_CreateInfo.ColorAttachments[Idx];
			auto& Attachment = m_Attachments[Idx];

			D3D12_RENDER_TARGET_VIEW_DESC ViewDesc = {};
			ViewDesc.Format = EGraphicsFormatToDXGIFormat(AttachmentInfo.Format);
			ViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

			ViewDesc.Texture2D.MipSlice = 0;
			ViewDesc.Texture2D.PlaneSlice = 0;

			for (size_t ImageIdx = 0; ImageIdx < Attachment.Images.size(); ImageIdx++)
			{
				D3D12_CPU_DESCRIPTOR_HANDLE Handle = m_RenderTargetViewHandle;
				Handle.ptr += (ImageIdx * m_Attachments.size() + Idx) * m_RenderTargetViewHandleIncrementSize;
				Device->CreateRenderTargetView(Attachment.Images[ImageIdx], &ViewDesc, Handle);
			}
		}

		// Depth Stencil Attachment
		{
			const auto& AttachmentInfo = m_CreateInfo.DepthAttachment;

			D3D12_HEAP_PROPERTIES HeapProperties = {};
			HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
			HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			HeapProperties.CreationNodeMask = 0;
			HeapProperties.VisibleNodeMask = 0;

			D3D12_RESOURCE_DESC1 ResourceDesc = {};
			ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			ResourceDesc.Alignment = 0;
			ResourceDesc.Width = m_CreateInfo.Width;
			ResourceDesc.Height = m_CreateInfo.Height;
			ResourceDesc.DepthOrArraySize = 1;
			ResourceDesc.MipLevels = 1;
			ResourceDesc.Format = EGraphicsFormatToDXGIFormat(AttachmentInfo.Format);
			ResourceDesc.SampleDesc.Count = 1;
			ResourceDesc.SampleDesc.Quality = 0;
			ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			ResourceDesc.Flags = EImageFlagsToD3D12ResourceFlags(AttachmentInfo.Flags);

			ResourceDesc.SamplerFeedbackMipRegion.Width = 0;
			ResourceDesc.SamplerFeedbackMipRegion.Height = 0;
			ResourceDesc.SamplerFeedbackMipRegion.Depth = 0;

			D3D12_CLEAR_VALUE ClearValue = {};
			ClearValue.Format = ResourceDesc.Format;
			ClearValue.DepthStencil.Depth = 1.0f;
			ClearValue.DepthStencil.Stencil = 0.0f;

			for (size_t ImageIdx = 0; ImageIdx < Context->GetFramesInFlight(); ImageIdx++)
			{
				Device->CreateCommittedResource2(
					&HeapProperties,
					D3D12_HEAP_FLAG_NONE,
					&ResourceDesc,
					EImageStatesToD3D12ResourceStates(m_DepthStencilAttachment.States[ImageIdx] & ~(ImageStates::CopySrc | ImageStates::CopyDst)),
					&ClearValue,
					nullptr,
					m_DepthStencilAttachment.Images[ImageIdx], m_DepthStencilAttachment.Images[ImageIdx]
				);
			}
		}

		if (IsDepthFormat(m_CreateInfo.DepthAttachment.Format))
		{
			D3D12_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc = {};
			DepthStencilViewDesc.Format = EGraphicsFormatToDXGIFormat(m_CreateInfo.DepthAttachment.Format);
			DepthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;
			DepthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			DepthStencilViewDesc.Texture2D.MipSlice = 0;

			for (size_t Idx = 0; Idx < Context->GetFramesInFlight(); Idx++)
			{
				D3D12_CPU_DESCRIPTOR_HANDLE Handle = m_DepthStencilViewHandle;
				Handle.ptr += Idx * m_DepthStencilViewHandleIncrementSize;
				Device->CreateDepthStencilView(m_DepthStencilAttachment.Images[Idx], &DepthStencilViewDesc, Handle);
			}
		}
	}

	void D3D12RenderTexture::TransitionImages(EImageState InColorAttachmentState, EImageState InDepthStencilState)
	{
		auto Context = GraphicsContext::Get<D3D12Context>();
		auto& Device = Context->GetDevice();
		auto& CmdList = static_cast<D3D12CommandList*>(Context->GetCurrentCommandList())->GetNativeList();

		// Transition Attachment Images
		uint32_t MaxBarrierCount = m_Attachments.size() + (m_DepthStencilDescriptorHeap.IsValid() ? 1 : 0);

		std::vector<D3D12_RESOURCE_BARRIER> Barriers;
		Barriers.reserve(MaxBarrierCount);
		uint32_t CurrentFrameIdx = Context->GetCurrentFrameIndex();

		for (size_t Idx = 0; Idx < m_Attachments.size(); Idx++)
		{
			if (m_Attachments[Idx].States[CurrentFrameIdx] == InColorAttachmentState)
				continue;

			auto& Barrier = Barriers.emplace_back();
			Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			Barrier.Transition.pResource = m_Attachments[Idx].Images[CurrentFrameIdx];
			Barrier.Transition.StateBefore = EImageStatesToD3D12ResourceStates(m_Attachments[Idx].States[CurrentFrameIdx]);
			Barrier.Transition.StateAfter = EImageStatesToD3D12ResourceStates(InColorAttachmentState);
			Barrier.Transition.Subresource = 0;

			m_Attachments[Idx].States[CurrentFrameIdx] = InColorAttachmentState;
		}

		// Depth Stencil
		if (m_DepthStencilDescriptorHeap.IsValid() && m_DepthStencilAttachment.States[CurrentFrameIdx] != InDepthStencilState)
		{
			auto& Barrier = Barriers.emplace_back();
			Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			Barrier.Transition.pResource = m_DepthStencilAttachment.Images[CurrentFrameIdx];
			Barrier.Transition.StateBefore = EImageStatesToD3D12ResourceStates(m_DepthStencilAttachment.States[CurrentFrameIdx]);
			Barrier.Transition.StateAfter = EImageStatesToD3D12ResourceStates(InDepthStencilState);
			Barrier.Transition.Subresource = 0;

			m_DepthStencilAttachment.States[CurrentFrameIdx] = InDepthStencilState;
		}

		CmdList->ResourceBarrier(Barriers.size(), Barriers.data());
	}

	void* D3D12RenderTexture::GetColorTextureID(uint32_t InColorAttachmentIdx) const
	{
		return nullptr;
	}

}
