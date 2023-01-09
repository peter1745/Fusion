#pragma once

#include "EditorWindow.hpp"

#include "UI/ImGuiRenderTextureImage.hpp"

#include <Fusion/Renderer/WorldRenderer.hpp>
#include <Fusion/Renderer/RenderTexture.hpp>

namespace FusionEditor {

	class ViewportWindowBase : public EditorWindow
	{
	public:
		ViewportWindowBase(const std::string& InTitle, const Fusion::Shared<Fusion::World>& InWorld);

		virtual void OnRender() override;
		virtual void OnUpdate(float InDeltaTime) override;

		virtual constexpr EMenuBarLocation GetMenuBarLocation() const override { return EMenuBarLocation::View; }

	protected:
		virtual void RenderContents() override;

	private:
		virtual void OnResize([[maybe_unused]] uint32_t InWidth, [[maybe_unused]] uint32_t InHeight) {}
		virtual void RenderWorld() = 0;

	protected:
		Fusion::Shared<Fusion::World> m_World = nullptr;
		Fusion::Unique<Fusion::WorldRenderer> m_WorldRenderer = nullptr;
		std::vector<Fusion::Shared<Fusion::RenderTexture>> m_RenderTextures;

		float m_MinRenderBoundX = 0;
		float m_MinRenderBoundY = 0;
		float m_RenderWidth = 300;
		float m_RenderHeight = 300;

		bool m_IsHovered = false;

		Fusion::Unique<ImGuiRenderTextureImage> m_ViewportImage = nullptr;

	};

}
