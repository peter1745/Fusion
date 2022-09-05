#pragma once

#include "Window.h"
#include "Fusion/Renderer/GraphicsContext.h"
#include "Fusion/Renderer/VertexBuffer.h"
#include "Fusion/Renderer/IndexBuffer.h"
#include "Fusion/Renderer/Shader.h"
#include "Fusion/Renderer/RenderCommandBuffer.h"

#include <chrono>

namespace Fusion {

	struct ApplicationSpecification
	{
		std::string_view Title;
		uint32_t WindowWidth = 1920;
		uint32_t WindowHeight = 1080;
	};

	class Application
	{
	public:
		using TimePoint = std::chrono::high_resolution_clock::time_point;

	public:
		Application(const ApplicationSpecification& InSpecification);
		virtual ~Application();

		void Run();

		const Unique<Window>& GetWindow() const { return m_Window; }

	public:
		static Application& Get();

	private:
		virtual void OnInit() {}
		virtual void OnUpdate(float DeltaTime) {}
		virtual void OnShutdown() {}

	private:
		ApplicationSpecification m_Specification;
		Unique<Window> m_Window = nullptr;
		bool m_Running = false;

		float m_FrameTime = 0.0f;
		float m_TimeStep = 0.0f;
		TimePoint m_LastFrameTime;

		Shared<GraphicsContext> m_GraphicsContext = nullptr;

		// TEMP
		Shared<VertexBuffer> m_VertexBuffer = nullptr;
		Shared<IndexBuffer> m_IndexBuffer = nullptr;
		Shared<Shader> m_Shader = nullptr;

		Shared<RenderCommandBuffer> m_CommandBuffer = nullptr;
	};

	Application* CreateApplication(int ArgC, char** ArgV);

}
