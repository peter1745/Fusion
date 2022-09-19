#pragma once

#include "EditorWindow.h"
#include "Fusion/Renderer/Framebuffer.h"

namespace FusionEditor {

	class ViewportWindow : public EditorWindow
	{
	public:
		ViewportWindow(const Fusion::Shared<Fusion::Framebuffer>& InFramebuffer);

	private:
		virtual void RenderContents() override;
		
	private:
		Fusion::Shared<Fusion::Framebuffer> m_Framebuffer;
	};

}
