#include "FusionPCH.hpp"
#include "D3D12RenderTexture.hpp"
#include "D3D12Context.hpp"

#include "Fusion/Renderer/Renderer.hpp"

namespace Fusion {

	D3D12RenderTexture::D3D12RenderTexture(const RenderTextureInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo)
	{
		auto Context = GraphicsContext::Get<D3D12Context>();
		auto& Device = Context->GetDevice().As<D3D12Device>()->GetDevice();

		m_Attachments.resize(InCreateInfo.ColorAttachments.size());

		uint32_t FrameCount = Renderer::GetCurrent().GetFramesInFlight();

		for (size_t Idx = 0; Idx < InCreateInfo.ColorAttachments.size(); Idx++)
		{
			const auto& AttachmentInfo = InCreateInfo.ColorAttachments[Idx];
			auto& Attachment = m_Attachments[Idx];

			Image2DInfo ImageInfo = {};
			ImageInfo.Size = { InCreateInfo.Width, InCreateInfo.Height };
			ImageInfo.Usage = EImageUsage::Attachment;
			ImageInfo.Format = AttachmentInfo.Format;
			ImageInfo.Flags = AttachmentInfo.Flags;
			ImageInfo.InitialState = AttachmentInfo.InitialState;
			ImageInfo.ClearColor = AttachmentInfo.ClearColor;

			for (size_t ImageIdx = 0; ImageIdx < FrameCount; ImageIdx++)
				Attachment.Images.push_back(Shared<D3D12Image2D>::Create(ImageInfo));
		}

		// NOTE(Peter): Probably better to have a really large heap for RTVs, and then simply allocate into it
		D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc = {};
		DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		DescriptorHeapDesc.NumDescriptors = FrameCount * m_Attachments.size();
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
			ViewDesc.Format = EFormatToDXGIFormat(AttachmentInfo.Format);
			ViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

			ViewDesc.Texture2D.MipSlice = 0;
			ViewDesc.Texture2D.PlaneSlice = 0;

			for (size_t ImageIdx = 0; ImageIdx < Attachment.Images.size(); ImageIdx++)
			{
				auto& Resource = Attachment.Images[ImageIdx]->GetResource();
				D3D12_CPU_DESCRIPTOR_HANDLE Handle = m_RenderTargetViewHandle;
				Handle.ptr += (ImageIdx * m_Attachments.size() + Idx) * m_RenderTargetViewHandleIncrementSize;
				Device->CreateRenderTargetView(Resource, &ViewDesc, Handle);
			}
		}

		m_HasDepthStencilAttachment = IsDepthFormat(InCreateInfo.DepthAttachment.Format);

		// Depth Stencil Attachment
		if (m_HasDepthStencilAttachment)
		{
			const auto& AttachmentInfo = InCreateInfo.DepthAttachment;

			Image2DInfo DepthImageInfo = {};
			DepthImageInfo.Size = { InCreateInfo.Width, InCreateInfo.Height };
			DepthImageInfo.Usage = EImageUsage::Attachment;
			DepthImageInfo.Format = AttachmentInfo.Format;
			DepthImageInfo.Flags = AttachmentInfo.Flags;
			DepthImageInfo.InitialState = AttachmentInfo.InitialState;

			for (size_t ImageIdx = 0; ImageIdx < FrameCount; ImageIdx++)
				m_DepthStencilAttachment.Images.push_back(Shared<D3D12Image2D>::Create(DepthImageInfo));

			// NOTE(Peter): Again, probably should have a global heap for DSVs that we allocate into
			DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			DescriptorHeapDesc.NumDescriptors = FrameCount;
			Device->CreateDescriptorHeap(&DescriptorHeapDesc, m_DepthStencilDescriptorHeap, m_DepthStencilDescriptorHeap);

			m_DepthStencilViewHandle = m_DepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			m_DepthStencilViewHandleIncrementSize = Device->GetDescriptorHandleIncrementSize(DescriptorHeapDesc.Type);

			D3D12_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc = {};
			DepthStencilViewDesc.Format = EFormatToDXGIFormat(InCreateInfo.DepthAttachment.Format);
			DepthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;
			DepthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			DepthStencilViewDesc.Texture2D.MipSlice = 0;

			for (size_t DepthImageIdx = 0; DepthImageIdx < FrameCount; DepthImageIdx++)
			{
				auto& Resource = m_DepthStencilAttachment.Images[DepthImageIdx]->GetResource();
				D3D12_CPU_DESCRIPTOR_HANDLE Handle = m_DepthStencilViewHandle;
				Handle.ptr += DepthImageIdx * m_DepthStencilViewHandleIncrementSize;
				Device->CreateDepthStencilView(Resource, &DepthStencilViewDesc, Handle);
			}
		}
	}

	void D3D12RenderTexture::Bind(CommandList* InCommandList)
	{
		// TODO(Peter): Maybe take in the desired frame index as a parameter?
		uint32_t CurrentFrameIdx = Renderer::GetCurrent().GetCurrentFrame();
		
		auto& CmdList = static_cast<D3D12CommandList*>(InCommandList)->GetNativeList();

		// Set render targets + depth stencil (if available)
		{
			auto RTVStart = m_RenderTargetViewHandle;
			RTVStart.ptr += CurrentFrameIdx * m_Attachments.size() * m_RenderTargetViewHandleIncrementSize;

			auto DepthStencilStart = m_DepthStencilViewHandle;
			DepthStencilStart.ptr += CurrentFrameIdx * m_DepthStencilViewHandleIncrementSize;

			CmdList->OMSetRenderTargets(m_Attachments.size(), &RTVStart, true, m_HasDepthStencilAttachment ? &DepthStencilStart : nullptr);
		}

		// Clear render targets
		for (size_t Idx = 0; Idx < m_Attachments.size(); Idx++)
		{
			const auto& ClearColor = m_CreateInfo.ColorAttachments[Idx].ClearColor;

			auto Handle = m_RenderTargetViewHandle;
			Handle.ptr += (CurrentFrameIdx * m_Attachments.size() + Idx) * m_RenderTargetViewHandleIncrementSize;
			CmdList->ClearRenderTargetView(Handle, &ClearColor[0], 0, nullptr);
		}

		// Clear depth stencil
		if (m_HasDepthStencilAttachment)
		{
			auto Handle = m_DepthStencilViewHandle;
			Handle.ptr += CurrentFrameIdx * m_DepthStencilViewHandleIncrementSize;
			CmdList->ClearDepthStencilView(Handle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		}
	}

	void D3D12RenderTexture::Unbind(CommandList* InCommandList) {}
	void D3D12RenderTexture::Clear() {}

	void D3D12RenderTexture::Resize(uint32_t InAttachmentIndex, uint32_t InFrameIndex, const ImageSize& InSize)
	{
		if (InSize.Width == 0 || InSize.Height == 0)
			return;

		// NOTE(Peter): Is it realistic for different attachment images to have different sizes?
		m_CreateInfo.Width = InSize.Width;
		m_CreateInfo.Height = InSize.Height;

		auto& Device = GraphicsContext::Get<D3D12Context>()->GetDevice().As<D3D12Device>()->GetDevice();

		// Resize attachment image
		{
			auto& Attachment = m_Attachments[InAttachmentIndex];
			const auto& AttachmentInfo = m_CreateInfo.ColorAttachments[InAttachmentIndex];

			Attachment.Images[InFrameIndex]->Resize(InSize);

			D3D12_RENDER_TARGET_VIEW_DESC ViewDesc = {};
			ViewDesc.Format = EFormatToDXGIFormat(AttachmentInfo.Format);
			ViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			ViewDesc.Texture2D.MipSlice = 0;
			ViewDesc.Texture2D.PlaneSlice = 0;

			D3D12_CPU_DESCRIPTOR_HANDLE Handle = m_RenderTargetViewHandle;
			Handle.ptr += (InFrameIndex * m_Attachments.size() + InAttachmentIndex) * m_RenderTargetViewHandleIncrementSize;
			Device->CreateRenderTargetView(Attachment.Images[InFrameIndex]->GetResource(), &ViewDesc, Handle);
		}

		// Resize depth stencil if it exists
		if (m_HasDepthStencilAttachment)
		{
			const auto& AttachmentInfo = m_CreateInfo.DepthAttachment;

			m_DepthStencilAttachment.Images[InFrameIndex]->Resize(InSize);

			D3D12_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc = {};
			DepthStencilViewDesc.Format = EFormatToDXGIFormat(m_CreateInfo.DepthAttachment.Format);
			DepthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;
			DepthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			DepthStencilViewDesc.Texture2D.MipSlice = 0;

			D3D12_CPU_DESCRIPTOR_HANDLE Handle = m_DepthStencilViewHandle;
			Handle.ptr += InFrameIndex * m_DepthStencilViewHandleIncrementSize;
			Device->CreateDepthStencilView(m_DepthStencilAttachment.Images[InFrameIndex]->GetResource(), &DepthStencilViewDesc, Handle);
		}
	}

	Shared<Image2D> D3D12RenderTexture::GetImage(uint32_t InAttachmentIndex, uint32_t InImageIndex) const
	{
		return m_Attachments[InAttachmentIndex].Images[InImageIndex];
	}

	void D3D12RenderTexture::TransitionImages(CommandList* InCommandList, EImageState InColorAttachmentState, EImageState InDepthStencilState)
	{
		auto Context = GraphicsContext::Get<D3D12Context>();
		auto& CmdList = static_cast<D3D12CommandList*>(InCommandList)->GetNativeList();
		uint32_t CurrentFrameIdx = Renderer::GetCurrent().GetCurrentFrame();

		// NOTE(Peter): We don't call Image2D::Transition here because it's more efficient to batch
		//				the barriers together, instead of doing them one at a time

		std::vector<D3D12_RESOURCE_BARRIER> Barriers;
		Barriers.reserve(m_Attachments.size());

		for (size_t Idx = 0; Idx < m_Attachments.size(); Idx++)
		{
			auto& Image = m_Attachments[Idx].Images[CurrentFrameIdx];

			if (Image->m_State == InColorAttachmentState)
				continue;

			auto& Barrier = Barriers.emplace_back();
			Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			Barrier.Transition.pResource = Image->m_Resource;
			Barrier.Transition.StateBefore = ImageStatesToD3D12ResourceStates(Image->m_State);
			Barrier.Transition.StateAfter = ImageStatesToD3D12ResourceStates(InColorAttachmentState);
			Barrier.Transition.Subresource = 0;

			Image->m_State = InColorAttachmentState;
		}

		if (m_HasDepthStencilAttachment)
		{
			auto& DepthImage = m_DepthStencilAttachment.Images[CurrentFrameIdx];

			if (DepthImage->m_State != InDepthStencilState)
			{
				auto& Barrier = Barriers.emplace_back();
				Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				Barrier.Transition.pResource = DepthImage->m_Resource;
				Barrier.Transition.StateBefore = ImageStatesToD3D12ResourceStates(DepthImage->m_State);
				Barrier.Transition.StateAfter = ImageStatesToD3D12ResourceStates(InDepthStencilState);
				Barrier.Transition.Subresource = 0;

				DepthImage->m_State = InDepthStencilState;
			}
		}

		if (Barriers.size() > 0)
			CmdList->ResourceBarrier(Barriers.size(), Barriers.data());
	}

	void* D3D12RenderTexture::GetColorTextureID(uint32_t InColorAttachmentIdx) const
	{
		return nullptr;
	}

}
