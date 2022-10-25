#pragma once

#include "Window.hpp"
#include "Fusion/Renderer/Renderer.hpp"

#include <chrono>
#include <mutex>

namespace Fusion {

	struct ApplicationSpecification
	{
		std::string Title;
		uint32_t WindowWidth = 1920;
		uint32_t WindowHeight = 1080;
	};

	class Application
	{
	public:
		using TimePoint = std::chrono::high_resolution_clock::time_point;

	public:
		Application(const ApplicationSpecification& InSpecification, void* InUserData = nullptr);
		virtual ~Application();

		void Run();

		void SetTitle(const std::string& InTitle)
		{
			m_Window->SetTitle(InTitle);
			m_Specification.Title = InTitle;
		}

		void SubmitToMainThread(const std::function<void()>& InFunction)
		{
			std::scoped_lock<std::mutex> Lock(m_MainThreadQueueMutex);
			m_MainThreadQueue.push_back(InFunction);
		}

		const Shared<Renderer>& GetRenderer() const { return m_Renderer; }
		const Unique<Window>& GetWindow() const { return m_Window; }

	public:
		static Application& Get();

	private:
		virtual void OnInit() {}
		virtual void OnUpdate(float DeltaTime) {}
		virtual void OnShutdown() {}

		void EventCallback(Event& InEvent);
		virtual void OnEvent(Event& InEvent) {}

		void ExecuteMainThreadQueue();

	private:
		ApplicationSpecification m_Specification;
		Unique<Window> m_Window = nullptr;
		bool m_Running = false;

		float m_FrameTime = 0.0f;
		float m_TimeStep = 0.0f;
		TimePoint m_LastFrameTime;

		std::mutex m_MainThreadQueueMutex;
		std::vector<std::function<void()>> m_MainThreadQueue;

	protected:
		Shared<Renderer> m_Renderer = nullptr;

	};

	Application* CreateApplication(int ArgC, char** ArgV);

}
