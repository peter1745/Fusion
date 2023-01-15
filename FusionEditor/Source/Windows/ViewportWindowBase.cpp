#include "ViewportWindowBase.hpp"

#include "Fusion/Core/Application.hpp"
#include "Fusion/Renderer/Renderer.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <ImGui/imgui_internal.h>

namespace FusionEditor {

	ViewportWindowBase::ViewportWindowBase(const std::string& InTitle, const Fusion::Shared<Fusion::World>& InWorld)
	    : EditorWindow(InTitle, 300, 300), m_World(InWorld)
	{
		m_WorldRenderer = Fusion::MakeUnique<Fusion::WorldRenderer>(InWorld);

		Fusion::RenderTextureAttachment ColorAttachment = {};
		ColorAttachment.Format = Fusion::EFormat::RGBA8Unorm;
		ColorAttachment.Flags = Fusion::EImageFlag::AllowRenderTarget;
		ColorAttachment.InitialState = Fusion::EImageState::RenderTarget;
		ColorAttachment.ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

		Fusion::RenderTextureAttachment ColorPickingAttachment = {};
		ColorPickingAttachment.Format = Fusion::EFormat::RG32UInt;
		ColorPickingAttachment.Flags = Fusion::EImageFlag::AllowRenderTarget;
		ColorPickingAttachment.InitialState = Fusion::EImageState::RenderTarget;
		ColorPickingAttachment.ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		ColorPickingAttachment.IsMappable = true;

		Fusion::RenderTextureInfo RenderTextureCreateInfo;
		RenderTextureCreateInfo.Width = 300U;
		RenderTextureCreateInfo.Height = 300U;
		RenderTextureCreateInfo.ColorAttachments = { ColorAttachment, ColorPickingAttachment };
		RenderTextureCreateInfo.DepthAttachment = { Fusion::EFormat::D24UnormS8UInt, Fusion::EImageFlag::AllowDepthStencil, Fusion::EImageState::DepthWrite };

		for (uint32_t Idx = 0; Idx < Fusion::Renderer::GetCurrent().GetFramesInFlight(); Idx++)
		{
			auto RenderTexture = Fusion::Shared<Fusion::RenderTexture>::Create(RenderTextureCreateInfo);
			m_RenderTextures.push_back(RenderTexture);
		}

		m_ViewportImage = ImGuiRenderTextureImage::Create(m_RenderTextures);
	}

	void ViewportWindowBase::OnRender()
	{
		Fusion::Viewport WindowViewport = {};
		WindowViewport.TopLeftX = 0.0f;
		WindowViewport.TopLeftY = 0.0f;
		WindowViewport.Width = m_RenderWidth;
		WindowViewport.Height = m_RenderHeight;
		WindowViewport.MinDepth = 0.0f;
		WindowViewport.MaxDepth = 1.0f;
		
		auto* CmdList = Fusion::Renderer::GetCurrent().GetCurrentCommandList();

		uint32_t FrameIndex = Fusion::Renderer::GetCurrent().GetCurrentFrame();

		m_RenderTextures[FrameIndex]->TransitionImages(CmdList, Fusion::EImageState::RenderTarget, Fusion::EImageState::DepthWrite);
		CmdList->SetViewports({ WindowViewport });
		m_RenderTextures[FrameIndex]->Bind(CmdList);
		m_RenderTextures[FrameIndex]->Clear();
		
		RenderWorld();

		m_RenderTextures[FrameIndex]->Unbind(CmdList);
		m_RenderTextures[FrameIndex]->TransitionImages(CmdList, Fusion::EImageState::PixelShaderResource, Fusion::EImageState::PixelShaderResource);
	}

	void ViewportWindowBase::OnUpdate([[maybe_unused]] float InDeltaTime)
	{
		uint32_t FrameIndex = Fusion::Renderer::GetCurrent().GetCurrentFrame();
		
		uint32_t ViewportWidth = m_RenderWidth;
		uint32_t ViewportHeight = m_RenderHeight;
		
		const auto& ImageSize = m_RenderTextures[FrameIndex]->GetImage(0)->GetSize();
		if (ViewportWidth != ImageSize.Width || ViewportHeight != ImageSize.Height)
		{
			m_ViewportImage->Resize(ViewportWidth, ViewportHeight);
			OnResize(ViewportWidth, ViewportHeight);
		}
	}

	void ViewportWindowBase::RenderContents()
	{
		ImVec2 MinBound = ImGui::GetCursorPos();
		ImVec2 MaxBound = ImGui::GetContentRegionMax();
		
		m_MinRenderBoundX = MinBound.x;
		m_MinRenderBoundY = ImGui::GetCursorStartPos().y + ImGui::GetCurrentWindow()->TitleBarHeight();
		m_RenderWidth = MaxBound.x - MinBound.x;
		m_RenderHeight = glm::max(0.0f, MaxBound.y - MinBound.y);

		m_ViewportImage->DrawImage(ImVec2(MaxBound.x - MinBound.x, MaxBound.y - MinBound.y));
	}

}
