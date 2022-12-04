#include "ViewportWindowBase.hpp"

#include "Fusion/Core/Application.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace FusionEditor {

	ViewportWindowBase::ViewportWindowBase(const std::string& InTitle, const Fusion::Shared<Fusion::World>& InWorld)
	    : EditorWindow(InTitle, 300, 300), m_World(InWorld)
	{
		m_WorldRenderer = Fusion::MakeUnique<Fusion::WorldRenderer>(InWorld);

		Fusion::RenderTextureAttachment ColorAttachment = {};
		ColorAttachment.Format = Fusion::EFormat::RGBA8Unorm;
		ColorAttachment.Flags = Fusion::ImageFlags::AllowRenderTarget;
		ColorAttachment.InitialState = Fusion::ImageStates::RenderTarget;
		ColorAttachment.ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

		Fusion::RenderTextureAttachment ColorPickingAttachment = {};
		ColorPickingAttachment.Format = Fusion::EFormat::RG32UInt;
		ColorPickingAttachment.Flags = Fusion::ImageFlags::AllowRenderTarget;
		ColorPickingAttachment.InitialState = Fusion::ImageStates::RenderTarget;
		ColorPickingAttachment.ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

		Fusion::RenderTextureInfo RenderTextureCreateInfo;
		RenderTextureCreateInfo.Width = 300U;
		RenderTextureCreateInfo.Height = 300U;
		RenderTextureCreateInfo.ColorAttachments = { ColorAttachment, ColorPickingAttachment };
		RenderTextureCreateInfo.DepthAttachment = { Fusion::EFormat::D24UnormS8UInt, Fusion::ImageFlags::AllowDepthStencil, Fusion::ImageStates::DepthWrite };
		m_RenderTexture = Fusion::RenderTexture::Create(RenderTextureCreateInfo);

		//auto Heap = Fusion::GraphicsContext::Get<Fusion::GraphicsContext>()->GetDescriptorHeap(Fusion::EDescriptorHeapType::SRV_CBV_UAV);
		//m_RTVAllocations = Heap->AllocateShaderResourceViews(m_RenderTexture, 0);
		//m_ColorPickingRTVAllocations = Heap->AllocateShaderResourceViews(m_RenderTexture, 1);
	}

	void ViewportWindowBase::OnRender()
	{
		//Fusion::Viewport WindowViewport = {};
		//WindowViewport.TopLeftX = 0.0f;
		//WindowViewport.TopLeftY = 0.0f;
		//WindowViewport.Width = m_RenderWidth;
		//WindowViewport.Height = m_RenderHeight;
		//WindowViewport.MinDepth = 0.0f;
		//WindowViewport.MaxDepth = 1.0f;
		//
		//auto* CmdList = Fusion::GraphicsContext::Get<Fusion::GraphicsContext>()->GetCurrentCommandList();
		//
		//m_RenderTexture->TransitionImages(CmdList, Fusion::ImageStates::RenderTarget, Fusion::ImageStates::DepthWrite);
		//CmdList->SetViewports({ WindowViewport });
		//m_RenderTexture->Bind(CmdList);
		//m_RenderTexture->Clear();
		//
		//RenderWorld();
		//
		//m_RenderTexture->Unbind(CmdList);
		//m_RenderTexture->TransitionImages(CmdList, Fusion::ImageStates::PixelShaderResource, Fusion::ImageStates::PixelShaderResource);
	}

	void ViewportWindowBase::OnUpdate([[maybe_unused]] float InDeltaTime)
	{
		//auto Context = Fusion::GraphicsContext::Get<Fusion::GraphicsContext>();
		//uint32_t FrameIndex = Context->GetCurrentFrameIndex();
		//
		//uint32_t ViewportWidth = m_RenderWidth;// GetWindowWidth();
		//uint32_t ViewportHeight = m_RenderHeight;// GetWindowHeight();
		//
		//const auto& ImageSize = m_RenderTexture->GetImage(0, FrameIndex)->GetSize();
		//if (ViewportWidth != ImageSize.Width || ViewportHeight != ImageSize.Height)
		//{
		//	auto Heap = Context->GetDescriptorHeap(Fusion::EDescriptorHeapType::SRV_CBV_UAV);
		//	Heap->Deallocate(m_RTVAllocations[FrameIndex]);
		//	Heap->Deallocate(m_ColorPickingRTVAllocations[FrameIndex]);
		//
		//	m_RenderTexture->Resize(0, FrameIndex, { ViewportWidth, ViewportHeight });
		//	m_RenderTexture->Resize(1, FrameIndex, { ViewportWidth, ViewportHeight });
		//
		//	m_RTVAllocations[FrameIndex] = Heap->AllocateShaderResourceView(m_RenderTexture, 0, FrameIndex);
		//	m_ColorPickingRTVAllocations[FrameIndex] = Heap->AllocateShaderResourceView(m_RenderTexture, 1, FrameIndex);
		//
		//	OnResize(ViewportWidth, ViewportHeight);
		//}
	}

	void ViewportWindowBase::RenderContents()
	{
		//ImVec2 MinBound = ImGui::GetCursorPos();
		//ImVec2 MaxBound = ImGui::GetContentRegionMax();
		//
		//m_MinRenderBoundX = MinBound.x;
		//m_MinRenderBoundY = MinBound.y;
		//m_RenderWidth = MaxBound.x - MinBound.x;
		//m_RenderHeight = MaxBound.y - MinBound.y;
		//
		//auto Context = Fusion::GraphicsContext::Get<Fusion::GraphicsContext>();
		//uint32_t FrameIdx = Context->GetCurrentFrameIndex();
		//auto Heap = Context->GetDescriptorHeap(Fusion::EDescriptorHeapType::SRV_CBV_UAV);
		//ImTextureID TexID = reinterpret_cast<ImTextureID>(Heap->GetGPUDescriptorHandle(m_RTVAllocations[FrameIdx]));
		//ImGui::Image(TexID, ImVec2(MaxBound.x - MinBound.x, MaxBound.y - MinBound.y));
	}

}
