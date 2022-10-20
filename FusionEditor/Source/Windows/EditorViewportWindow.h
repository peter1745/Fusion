#pragma once

#include "ViewportWindowBase.h"
#include "Renderer/ViewportCamera.h"

namespace FusionEditor {

	class EditorViewportWindow : public ViewportWindowBase
	{
	public:
		EditorViewportWindow(const Fusion::Shared<Fusion::World>& InWorld);

		virtual void OnUpdate(float InDeltaTime) override;

	private:
		virtual void RenderWorld() override;
		virtual void OnResize(uint32_t InWidth, uint32_t InHeight) override;

	private:
		ViewportCamera m_Camera;

	};

}
