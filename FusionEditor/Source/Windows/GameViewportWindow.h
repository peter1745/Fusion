#pragma once

#include "ViewportWindowBase.h"

namespace FusionEditor {

	class GameViewportWindow : public ViewportWindowBase
	{
	public:
		GameViewportWindow(Fusion::World* InWorld);

	private:
		virtual void RenderWorld() override;
		virtual void OnResize(uint32_t InWidth, uint32_t InHeight) override;

	};

}
