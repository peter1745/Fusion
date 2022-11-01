#include "ViewportWindowBase.hpp"

#include "Fusion/Core/Application.hpp"
#include "Platform/D3D12/D3D12DescriptorHeap.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace FusionEditor {

	ViewportWindowBase::ViewportWindowBase(const std::string& InTitle, const Fusion::Shared<Fusion::World>& InWorld, Fusion::DescriptorHeap* InDescriptorHeap)
		: EditorWindow(InTitle, 300, 300), m_World(InWorld), m_ViewportWidth(300), m_ViewportHeight(300), m_DescriptorHeap(InDescriptorHeap)
	{
		m_WorldRenderer = Fusion::MakeUnique<Fusion::WorldRenderer>(InWorld);

		Fusion::RenderTextureInfo RenderTextureCreateInfo;
		RenderTextureCreateInfo.Width = uint32_t(m_ViewportWidth);
		RenderTextureCreateInfo.Height = uint32_t(m_ViewportHeight);
		RenderTextureCreateInfo.ColorAttachments = {
			{ Fusion::EGraphicsFormat::RGBA8Unorm, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), Fusion::ImageStates::RenderTarget, Fusion::ImageFlags::AllowRenderTarget }
		};
		RenderTextureCreateInfo.DepthAttachment = { Fusion::EGraphicsFormat::D24UnormS8UInt, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), Fusion::ImageStates::DepthWrite, Fusion::ImageFlags::AllowDepthStencil };
		m_RenderTexture = Fusion::RenderTexture::Create(RenderTextureCreateInfo);

		m_RTVAllocations = InDescriptorHeap->AllocateRenderTextureViews(m_RenderTexture, 0);
	}

	void ViewportWindowBase::OnRender()
	{
		Fusion::Viewport WindowViewport = {};
		WindowViewport.TopLeftX = 0.0f;
		WindowViewport.TopLeftY = 0.0f;
		WindowViewport.Width = m_ViewportWidth;
		WindowViewport.Height = m_ViewportHeight;
		WindowViewport.MinDepth = 0.0f;
		WindowViewport.MaxDepth = 1.0f;

		auto* CmdList = Fusion::GraphicsContext::Get<Fusion::GraphicsContext>()->GetCurrentCommandList();

		m_RenderTexture->TransitionImages(Fusion::ImageStates::RenderTarget, Fusion::ImageStates::DepthWrite);
		CmdList->SetViewports({ WindowViewport });
		m_RenderTexture->Bind();
		m_RenderTexture->Clear();

		RenderWorld();
		
		m_RenderTexture->Unbind();
		m_RenderTexture->TransitionImages(Fusion::ImageStates::PixelShaderResource, Fusion::ImageStates::PixelShaderResource);
	}

	void ViewportWindowBase::OnUpdate([[maybe_unused]] float InDeltaTime)
	{
		uint32_t NewWidth = GetWindowWidth();
		uint32_t NewHeight = GetWindowHeight();

		if (m_ViewportWidth != NewWidth || m_ViewportHeight != NewHeight)
		{
			if (NewWidth == 0 || NewHeight == 0)
				return;

			m_ViewportWidth = NewWidth;
			m_ViewportHeight = NewHeight;

			m_RenderTexture->Resize(m_ViewportWidth, m_ViewportHeight);
			m_RTVAllocations = m_DescriptorHeap->AllocateRenderTextureViews(m_RenderTexture, 0);

			OnResize(m_ViewportWidth, m_ViewportHeight);
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
