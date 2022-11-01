#pragma once

#include <Fusion/Core/Window.hpp>
#include <Fusion/Renderer/GraphicsContext.hpp>
#include <Fusion/Renderer/DescriptorHeap.hpp>

#include <memory>

namespace FusionEditor {

	class ImGuiPlatformContext
	{
	public:
		virtual ~ImGuiPlatformContext() = default;

		void Init(const Fusion::Unique<Fusion::Window>& InWindow, const Fusion::Shared<Fusion::GraphicsContext>& InContext, Fusion::DescriptorHeap* InDescriptorHeap);
		void BeginFrame();
		void EndFrame();
		void Shutdown();

		static std::unique_ptr<ImGuiPlatformContext> Create();

	private:
		virtual void InitPlatform(const Fusion::Unique<Fusion::Window>& InWindow, const Fusion::Shared<Fusion::GraphicsContext>& InContext, Fusion::DescriptorHeap* InDescriptorHeap) = 0;
		virtual void BeginFramePlatform() = 0;
		virtual void EndFramePlatform() = 0;
		virtual void ShutdownPlatform() = 0;

		void InitStyle();
	};

}
