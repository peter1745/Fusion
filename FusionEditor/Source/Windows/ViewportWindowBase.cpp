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
		m_RenderTexture = Fusion::RenderTexture::Create(RenderTextureCreateInfo);
	}

	void ViewportWindowBase::OnRender()
	{
		static const glm::vec4 s_ClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		m_RenderTexture->Bind();
		m_RenderTexture->Clear(s_ClearColor);
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
		ImGui::Image(m_RenderTexture->GetColorTextureID(), ImVec2(float(m_ViewportWidth), float(m_ViewportHeight)));
	}


}
