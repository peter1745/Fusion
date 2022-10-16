#include "ViewportWindowBase.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace FusionEditor {

	ViewportWindowBase::ViewportWindowBase(const std::string& InWindowID, Fusion::World* InWorld)
		: EditorWindow("GameViewport", 500, 300), m_World(InWorld), m_ViewportWidth(500.0f), m_ViewportHeight(300.0f)
	{
		m_WorldRenderer = Fusion::MakeUnique<Fusion::WorldRenderer>(InWorld);

		Fusion::RenderTextureInfo RenderTextureCreateInfo;
		RenderTextureCreateInfo.Width = uint32_t(m_ViewportWidth);
		RenderTextureCreateInfo.Height = uint32_t(m_ViewportHeight);
		m_RenderTexture = Fusion::RenderTexture::Create(RenderTextureCreateInfo);
	}

	void ViewportWindowBase::OnRender()
	{
		static const glm::vec4 s_ClearColor = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);

		m_RenderTexture->Bind();
		m_RenderTexture->Clear(s_ClearColor);
		RenderWorld();
		m_RenderTexture->Unbind();
	}

	void ViewportWindowBase::OnUpdate(float InDeltaTime)
	{
		if (m_ViewportWidth != GetWindowWidth() || m_ViewportHeight != GetWindowHeight())
		{
			m_ViewportWidth = GetWindowWidth();
			m_ViewportHeight = GetWindowHeight();
			
			m_RenderTexture->Resize(m_ViewportWidth, m_ViewportHeight);

			OnResize(m_ViewportWidth, m_ViewportHeight);
		}
	}

	void ViewportWindowBase::RenderContents()
	{
		ImGui::Image(m_RenderTexture->GetColorTextureID(), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
	}


}
