#include "ViewportWindowBase.h"

#include "Fusion/Core/Application.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace FusionEditor {

	ViewportWindowBase::ViewportWindowBase(const std::string& InWindowID, Fusion::World* InWorld)
		: EditorWindow(InWindowID, 300, 300), m_World(InWorld), m_ViewportWidth(300.0f), m_ViewportHeight(300.0f)
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
		const Fusion::Unique<Fusion::Window>& Window = Fusion::Application::Get().GetWindow();

#if 1
		uint32_t NewWidth = GetWindowWidth();
		uint32_t NewHeight = GetWindowHeight();
#else
		uint32_t NewWidth = Window->GetWidth();
		uint32_t NewHeight = Window->GetHeight();
#endif
		if (m_ViewportWidth != NewWidth || m_ViewportHeight != NewHeight)
		{
			//if (GetWindowWidth() == 0 || GetWindowHeight() == 0)
			//	return;

			m_ViewportWidth = NewWidth;
			m_ViewportHeight = NewHeight;

			m_RenderTexture->Resize(m_ViewportWidth, m_ViewportHeight);

			OnResize(m_ViewportWidth, m_ViewportHeight);
		}
	}

	void ViewportWindowBase::RenderContents()
	{
		ImGui::Image(m_RenderTexture->GetColorTextureID(), ImVec2(GetWindowWidth(), GetWindowHeight()));
	}


}
