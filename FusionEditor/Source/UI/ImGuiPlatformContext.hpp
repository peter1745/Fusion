#pragma once

#include <Fusion/Core/Window.hpp>
#include <Fusion/Renderer/GraphicsContext.hpp>
#include <Fusion/Renderer/SwapChain.hpp>
#include <Fusion/Renderer/DescriptorHeap.hpp>
#include <Fusion/Renderer/CommandList.hpp>

#include <memory>

namespace FusionEditor {

	class ImGuiPlatformContext
	{
	public:
		virtual ~ImGuiPlatformContext() = default;

		void Init(const Fusion::Unique<Fusion::Window>& InWindow, const Fusion::Shared<Fusion::GraphicsContext>& InContext, const Fusion::Shared<Fusion::SwapChain>& InSwapChain);
		void BeginFrame(Fusion::CommandList* InCommandList);
		void EndFrame(Fusion::CommandList* InCommandList);
		void Shutdown();

		static std::unique_ptr<ImGuiPlatformContext> Create();

	private:
		virtual void InitPlatform(const Fusion::Unique<Fusion::Window>& InWindow, const Fusion::Shared<Fusion::GraphicsContext>& InContext, const Fusion::Shared<Fusion::SwapChain>& InSwapChain) = 0;
		virtual void BeginFramePlatform(Fusion::CommandList* InCommandList) = 0;
		virtual void EndFramePlatform(Fusion::CommandList* InCommandList) = 0;
		virtual void ShutdownPlatform() = 0;

		void InitStyle();
	};

}
