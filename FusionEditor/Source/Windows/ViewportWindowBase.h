#pragma once

#include "EditorWindow.h"

#include <Fusion/Renderer/WorldRenderer.h>
#include <Fusion/Renderer/RenderTexture.h>

namespace FusionEditor {

	class ViewportWindowBase : public EditorWindow
	{
	public:
		ViewportWindowBase(const std::string& InWindowID, const Fusion::Shared<Fusion::World>& InWorld);

		virtual void OnRender() override;
		virtual void OnUpdate(float InDeltaTime) override;

	protected:
		virtual void RenderContents() override;

	private:
		virtual void OnResize(uint32_t InWidth, uint32_t InHeight) {}
		virtual void RenderWorld() = 0;

	protected:
		Fusion::Shared<Fusion::World> m_World = nullptr;
		Fusion::Unique<Fusion::WorldRenderer> m_WorldRenderer = nullptr;
		Fusion::Shared<Fusion::RenderTexture> m_RenderTexture = nullptr;
		float m_ViewportWidth = 0.0f, m_ViewportHeight = 0.0f;

		bool m_IsHovered = false;
	};

}
