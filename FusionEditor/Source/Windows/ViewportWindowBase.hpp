#pragma once

#include "EditorWindow.hpp"

#include <Fusion/Renderer/WorldRenderer.hpp>
#include <Fusion/Renderer/RenderTexture.hpp>
#include <Fusion/Renderer/DescriptorHeap.hpp>

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
		Fusion::Shared<Fusion::RenderTexture> m_RenderTexture = nullptr;

		bool m_IsHovered = false;

		std::vector<Fusion::DescriptorHeapAllocation> m_RTVAllocations;
		std::vector<Fusion::DescriptorHeapAllocation> m_ColorPickingRTVAllocations;

	};

}
