#include "ViewportWindowBase.hpp"

#include "Fusion/Core/Application.hpp"
#include "Platform/D3D12/D3D12DescriptorHeap.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace FusionEditor {

	ViewportWindowBase::ViewportWindowBase(const std::string& InTitle, const Fusion::Shared<Fusion::World>& InWorld, Fusion::DescriptorHeap* InDescriptorHeap)
		: EditorWindow(InTitle, 300, 300), m_World(InWorld), m_DescriptorHeap(InDescriptorHeap)
	{
		m_WorldRenderer = Fusion::MakeUnique<Fusion::WorldRenderer>(InWorld);

		Fusion::RenderTextureAttachment ColorAttachment = {};
		ColorAttachment.Format = Fusion::EFormat::RGBA8Unorm;
		ColorAttachment.Flags = Fusion::ImageFlags::AllowRenderTarget;
		ColorAttachment.InitialState = Fusion::ImageStates::RenderTarget;
		ColorAttachment.ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

		Fusion::RenderTextureInfo RenderTextureCreateInfo;
		RenderTextureCreateInfo.Width = 300U;
		RenderTextureCreateInfo.Height = 300U;
		RenderTextureCreateInfo.ColorAttachments = { ColorAttachment };
		RenderTextureCreateInfo.DepthAttachment = { Fusion::EFormat::D24UnormS8UInt, Fusion::ImageFlags::AllowDepthStencil, Fusion::ImageStates::DepthWrite };
		m_RenderTexture = Fusion::RenderTexture::Create(RenderTextureCreateInfo);

		m_RTVAllocations = InDescriptorHeap->AllocateRenderTextureViews(m_RenderTexture, 0);
	}

	void ViewportWindowBase::OnRender()
	{
		Fusion::Viewport WindowViewport = {};
		WindowViewport.TopLeftX = 0.0f;
		WindowViewport.TopLeftY = 0.0f;
		WindowViewport.Width = GetWindowWidth();
		WindowViewport.Height = GetWindowHeight();
		WindowViewport.MinDepth = 0.0f;
		WindowViewport.MaxDepth = 1.0f;

		auto* CmdList = Fusion::GraphicsContext::Get<Fusion::GraphicsContext>()->GetCurrentCommandList();

		m_RenderTexture->TransitionImages(CmdList, Fusion::ImageStates::RenderTarget, Fusion::ImageStates::DepthWrite);
		CmdList->SetViewports({ WindowViewport });
		m_RenderTexture->Bind(CmdList);
		m_RenderTexture->Clear();

		RenderWorld();
		
		m_RenderTexture->Unbind(CmdList);
		m_RenderTexture->TransitionImages(CmdList, Fusion::ImageStates::PixelShaderResource, Fusion::ImageStates::PixelShaderResource);
	}

	void ViewportWindowBase::OnUpdate([[maybe_unused]] float InDeltaTime)
	{
		uint32_t FrameIndex = Fusion::GraphicsContext::Get<Fusion::GraphicsContext>()->GetCurrentFrameIndex();

		uint32_t ViewportWidth = GetWindowWidth();
		uint32_t ViewportHeight = GetWindowHeight();

		const auto& ImageSize = m_RenderTexture->GetImage(0, FrameIndex)->GetSize();
		if (ViewportWidth != ImageSize.Width || ViewportHeight != ImageSize.Height)
		{
			m_DescriptorHeap->Deallocate(m_RTVAllocations[FrameIndex].Index);
			m_RenderTexture->Resize(0, FrameIndex, { ViewportWidth, ViewportHeight });
			m_RTVAllocations[FrameIndex] = m_DescriptorHeap->AllocateRenderTextureView(m_RenderTexture, 0, FrameIndex);

			OnResize(ViewportWidth, ViewportHeight);
		}
	}

	void ViewportWindowBase::RenderContents()
	{
		ImVec2 MinBound = GetMinBound();
		ImVec2 MaxBound = GetMaxBound();

		uint32_t FrameIdx = Fusion::GraphicsContext::Get<Fusion::GraphicsContext>()->GetCurrentFrameIndex();
		auto Handle = static_cast<Fusion::D3D12DescriptorHeap*>(m_DescriptorHeap)->GetGPUDescriptorHandle(m_RTVAllocations[FrameIdx].Index);
		ImTextureID TexID = reinterpret_cast<ImTextureID>(Handle.ptr);
		ImGui::Image(TexID, ImVec2(MaxBound.x - MinBound.x, MaxBound.y - MinBound.y));
	}

}
