#include "ViewportWindowBase.hpp"

#include "Fusion/Core/Application.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace FusionEditor {

	ViewportWindowBase::ViewportWindowBase(const std::string& InTitle, const Fusion::Shared<Fusion::World>& InWorld)
		: EditorWindow(InTitle, 300, 300), m_World(InWorld), m_ViewportWidth(300), m_ViewportHeight(300)
	{
		m_WorldRenderer = Fusion::MakeUnique<Fusion::WorldRenderer>(InWorld);

		Fusion::RenderTextureInfo RenderTextureCreateInfo;
		RenderTextureCreateInfo.Width = uint32_t(m_ViewportWidth);
		RenderTextureCreateInfo.Height = uint32_t(m_ViewportHeight);
		RenderTextureCreateInfo.ColorAttachments = {
			{ Fusion::ERenderTextureAttachmentFormat::RGBA8 },
			{ Fusion::ERenderTextureAttachmentFormat::R32G32UInt, true, glm::vec4(1.0f) }
		};

		m_RenderTexture = Fusion::RenderTexture::Create(RenderTextureCreateInfo);
	}

	void ViewportWindowBase::OnRender()
	{
		m_RenderTexture->Bind();
		m_RenderTexture->Clear();
		RenderWorld();
		m_RenderTexture->Unbind();
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

			OnResize(m_ViewportWidth, m_ViewportHeight);
		}
	}

	void ViewportWindowBase::RenderContents()
	{
		ImVec2 MinBound = GetMinBound();
		ImVec2 MaxBound = GetMaxBound();

		ImGui::Image(m_RenderTexture->GetColorTextureID(0), ImVec2(MaxBound.x - MinBound.x, MaxBound.y - MinBound.y));
	}

}
