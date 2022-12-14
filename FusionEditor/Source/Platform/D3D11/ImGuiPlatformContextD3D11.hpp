#pragma once

#include "UI/ImGuiPlatformContext.hpp"

namespace FusionEditor {

	class ImGuiPlatformContextD3D11 : public ImGuiPlatformContext
	{
	private:
		virtual void InitPlatform(const Fusion::Unique<Fusion::Window>& InWindow, const Fusion::Shared<Fusion::GraphicsContext>& InContext) override;
		virtual void BeginFramePlatform() override;
		virtual void EndFramePlatform() override;
		virtual void ShutdownPlatform() override;

	};

}
