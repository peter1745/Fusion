#pragma once

#include <Platform/D3D12/D3D12Common.hpp>

#include "UI/ImGuiPlatformContext.hpp"

namespace FusionEditor {

	class ImGuiPlatformContextD3D12 : public ImGuiPlatformContext
	{
	private:
		virtual void InitPlatform(const Fusion::Unique<Fusion::Window>& InWindow, const Fusion::Shared<Fusion::GraphicsContext>& InContext) override;
		virtual void BeginFramePlatform() override;
		virtual void EndFramePlatform() override;
		virtual void ShutdownPlatform() override;

	private:
		Fusion::D3D12ComPtr<ID3D12DescriptorHeap> m_ImGuiDescriptorHeap;
	};

}
