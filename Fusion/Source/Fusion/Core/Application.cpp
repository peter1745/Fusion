#include "FusionPCH.h"
#include "Application.h"

#include "Fusion/IO/Keyboard.h"
#include "Fusion/IO/Mouse.h"
#include "Fusion/Renderer/Renderer.h"
#include "Fusion/Events/WindowEvents.h"

#include <glm/glm.hpp>

namespace Fusion {

	static Application* s_Application = nullptr;

	Application::Application(const ApplicationSpecification& InSpecification)
		: m_Specification(InSpecification)
	{
		FUSION_CORE_VERIFY(s_Application == nullptr);
		s_Application = this;

		WindowSpecification WindowSpec = {};
		WindowSpec.Title = m_Specification.Title;
		WindowSpec.Width = m_Specification.WindowWidth;
		WindowSpec.Height = m_Specification.WindowHeight;
		m_Window = MakeUnique<Window>(WindowSpec);
		m_Window->Init();
		m_Window->SetEventCallback(FUSION_BIND_FUNC(Application::OnEvent));

		m_Renderer = Renderer::Create(ERendererAPI::D3D11);

		m_Window->Maximize();
	}

	Application::~Application()
	{
		m_Renderer = nullptr;
		m_Window = nullptr;
	}

	void Application::Run()
	{
		m_Running = true;
		OnInit();
		while (m_Running)
		{
			Keyboard::Get().TransitionHeldKeys();
			Mouse::Get().TransitionHeldButtons();
			m_Window->ProcessEvents();

			m_Renderer->Begin();
			OnUpdate(m_TimeStep);
			m_Renderer->End();
			m_Renderer->Present();

			TimePoint time = std::chrono::high_resolution_clock::now();
			m_FrameTime = std::chrono::duration_cast<std::chrono::duration<float>>(time - m_LastFrameTime).count();
			m_TimeStep = glm::min<float>(m_FrameTime, 0.0333f);
			m_LastFrameTime = time;
			
			Mouse::Get().ResetReleasedButtons();
			Keyboard::Get().ResetReleasedKeys();

			// TODO(Peter): Handle with event
			m_Running = !m_Window->ShouldClose();
		}
		OnShutdown();
	}

	Application& Application::Get() { return *s_Application; }

	void Application::OnEvent(Event& InEvent)
	{
		EventDispatcher Dispatcher(InEvent);
		Dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& InCloseEvent)
		{
			m_Running = false;
			return true;
		});

		Dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& InResizeEvent)
		{
			m_Renderer->OnResize(InResizeEvent.GetWidth(), InResizeEvent.GetHeight());
			return false;
		});
	}

}
