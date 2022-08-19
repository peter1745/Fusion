#pragma once

#include "Window.h"

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
	};

	Application* CreateApplication(int ArgC, char** ArgV);

}
