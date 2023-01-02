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

		for (size_t Idx = 0; Idx < InCreateInfo.ColorAttachments.size(); Idx++)
		{
			const auto& AttachmentInfo = InCreateInfo.ColorAttachments[Idx];

			Image2DInfo ImageInfo = {};
			ImageInfo.Size = { InCreateInfo.Width, InCreateInfo.Height };
			ImageInfo.Usage = EImageUsage::Attachment;
			ImageInfo.Format = AttachmentInfo.Format;
			ImageInfo.Flags = AttachmentInfo.Flags;
			ImageInfo.InitialState = AttachmentInfo.InitialState;
			ImageInfo.ClearColor = AttachmentInfo.ClearColor;
			m_Attachments.push_back(Shared<D3D12Image2D>::Create(ImageInfo));
		}

		// NOTE(Peter): Probably better to have a really large heap for RTVs, and then simply allocate into it
		D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc = {};
		DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		DescriptorHeapDesc.NumDescriptors = m_Attachments.size();
		DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		DescriptorHeapDesc.NodeMask = 0;
		Device->CreateDescriptorHeap(&DescriptorHeapDesc, m_RenderTargetDescriptorHeap, m_RenderTargetDescriptorHeap);

		m_RenderTargetViewHandle = m_RenderTargetDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		m_RenderTargetViewHandleIncrementSize = Device->GetDescriptorHandleIncrementSize(DescriptorHeapDesc.Type);

		for (size_t Idx = 0; Idx < m_Attachments.size(); Idx++)
		{
			const auto& AttachmentInfo = InCreateInfo.ColorAttachments[Idx];

			D3D12_RENDER_TARGET_VIEW_DESC ViewDesc = {};
			ViewDesc.Format = EFormatToDXGIFormat(AttachmentInfo.Format);
			ViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			ViewDesc.Texture2D.MipSlice = 0;
			ViewDesc.Texture2D.PlaneSlice = 0;

			auto& Resource = m_Attachments[Idx]->GetResource();
			D3D12_CPU_DESCRIPTOR_HANDLE Handle = m_RenderTargetViewHandle;
			Handle.ptr += Idx * m_RenderTargetViewHandleIncrementSize;
			Device->CreateRenderTargetView(Resource, &ViewDesc, Handle);
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
			m_DepthStencilAttachment = Shared<D3D12Image2D>::Create(DepthImageInfo);

			// NOTE(Peter): Again, probably should have a global heap for DSVs that we allocate into
			DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			DescriptorHeapDesc.NumDescriptors = 1;
			Device->CreateDescriptorHeap(&DescriptorHeapDesc, m_DepthStencilDescriptorHeap, m_DepthStencilDescriptorHeap);

			m_DepthStencilViewHandle = m_DepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			m_DepthStencilViewHandleIncrementSize = Device->GetDescriptorHandleIncrementSize(DescriptorHeapDesc.Type);

			D3D12_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc = {};
			DepthStencilViewDesc.Format = EFormatToDXGIFormat(InCreateInfo.DepthAttachment.Format);
			DepthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;
			DepthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			DepthStencilViewDesc.Texture2D.MipSlice = 0;

			Device->CreateDepthStencilView(m_DepthStencilAttachment->GetResource(), &DepthStencilViewDesc, m_DepthStencilViewHandle);
		}
	}

	void D3D12RenderTexture::Bind(CommandList* InCommandList)
	{
		auto& CmdList = static_cast<D3D12CommandList*>(InCommandList)->GetNativeList();

		// Set render targets + depth stencil (if available)
		{
			auto RTVStart = m_RenderTargetViewHandle;
			auto DepthStencilStart = m_DepthStencilViewHandle;
			CmdList->OMSetRenderTargets(m_Attachments.size(), &RTVStart, true, m_HasDepthStencilAttachment ? &DepthStencilStart : nullptr);
		}

		// Clear render targets
		for (size_t Idx = 0; Idx < m_Attachments.size(); Idx++)
		{
			const auto& ClearColor = m_CreateInfo.ColorAttachments[Idx].ClearColor;

			auto Handle = m_RenderTargetViewHandle;
			Handle.ptr += Idx * m_RenderTargetViewHandleIncrementSize;
			CmdList->ClearRenderTargetView(Handle, &ClearColor[0], 0, nullptr);
		}

		// Clear depth stencil
		if (m_HasDepthStencilAttachment)
			CmdList->ClearDepthStencilView(m_DepthStencilViewHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	}

	void D3D12RenderTexture::Unbind(CommandList* InCommandList) {}
	void D3D12RenderTexture::Clear() {}

	void D3D12RenderTexture::Resize(uint32_t InAttachmentIndex, const ImageSize& InSize)
	{
		if (InSize.Width == 0 || InSize.Height == 0)
			return;

		// NOTE(Peter): Is it realistic for different attachment images to have different sizes?
		m_CreateInfo.Width = InSize.Width;
		m_CreateInfo.Height = InSize.Height;

		auto& Device = GraphicsContext::Get<D3D12Context>()->GetDevice().As<D3D12Device>()->GetDevice();

		// Resize attachment image
		{
			const auto& AttachmentInfo = m_CreateInfo.ColorAttachments[InAttachmentIndex];

			m_Attachments[InAttachmentIndex]->Resize(InSize);

			D3D12_RENDER_TARGET_VIEW_DESC ViewDesc = {};
			ViewDesc.Format = EFormatToDXGIFormat(AttachmentInfo.Format);
			ViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			ViewDesc.Texture2D.MipSlice = 0;
			ViewDesc.Texture2D.PlaneSlice = 0;

			D3D12_CPU_DESCRIPTOR_HANDLE Handle = m_RenderTargetViewHandle;
			Handle.ptr += InAttachmentIndex * m_RenderTargetViewHandleIncrementSize;
			Device->CreateRenderTargetView(m_Attachments[InAttachmentIndex]->GetResource(), &ViewDesc, Handle);
		}

		// Resize depth stencil if it exists
		if (m_HasDepthStencilAttachment)
		{
			const auto& AttachmentInfo = m_CreateInfo.DepthAttachment;

			m_DepthStencilAttachment->Resize(InSize);

			D3D12_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc = {};
			DepthStencilViewDesc.Format = EFormatToDXGIFormat(m_CreateInfo.DepthAttachment.Format);
			DepthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;
			DepthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			DepthStencilViewDesc.Texture2D.MipSlice = 0;

			Device->CreateDepthStencilView(m_DepthStencilAttachment->GetResource(), &DepthStencilViewDesc, m_DepthStencilViewHandle);
		}
	}

	void D3D12RenderTexture::TransitionImages(CommandList* InCommandList, EImageState InColorAttachmentState, EImageState InDepthStencilState)
	{
		auto Context = GraphicsContext::Get<D3D12Context>();
		auto& CmdList = static_cast<D3D12CommandList*>(InCommandList)->GetNativeList();

		// NOTE(Peter): We don't call Image2D::Transition here because it's more efficient to batch
		//				the barriers together, instead of doing them one at a time

		std::vector<D3D12_RESOURCE_BARRIER> Barriers;
		Barriers.reserve(m_Attachments.size());

		for (size_t Idx = 0; Idx < m_Attachments.size(); Idx++)
		{
			auto Image = m_Attachments[Idx];

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
			if (m_DepthStencilAttachment->m_State != InDepthStencilState)
			{
				auto& Barrier = Barriers.emplace_back();
				Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				Barrier.Transition.pResource = m_DepthStencilAttachment->m_Resource;
				Barrier.Transition.StateBefore = ImageStatesToD3D12ResourceStates(m_DepthStencilAttachment->m_State);
				Barrier.Transition.StateAfter = ImageStatesToD3D12ResourceStates(InDepthStencilState);
				Barrier.Transition.Subresource = 0;

				m_DepthStencilAttachment->m_State = InDepthStencilState;
			}
		}

		if (Barriers.size() > 0)
			CmdList->ResourceBarrier(Barriers.size(), Barriers.data());
	}

	void* D3D12RenderTexture::GetColorTextureID(uint32_t InColorAttachmentIdx) const
	{
		return nullptr;
	}

	void D3D12RenderTexture::Release()
	{
		m_RenderTargetDescriptorHeap.Release();
		m_DepthStencilDescriptorHeap.Release();

		for (auto& Attachment : m_Attachments)
			Attachment->Release();
		m_Attachments.clear();

		if (m_HasDepthStencilAttachment)
			m_DepthStencilAttachment->Release();
		m_DepthStencilAttachment = nullptr;

		m_RenderTargetViewHandle.ptr = 0;
		m_DepthStencilViewHandle.ptr = 0;
	}

}
