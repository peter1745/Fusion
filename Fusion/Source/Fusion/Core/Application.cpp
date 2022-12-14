#include "FusionPCH.hpp"
#include "Application.hpp"

#include "Fusion/IO/Keyboard.hpp"
#include "Fusion/IO/Mouse.hpp"
#include "Fusion/Events/WindowEvents.hpp"

#include "Fusion/AssetSystem/AssetLoader.hpp"

#include <tracy/Tracy.hpp>

#include <glm/glm.hpp>

namespace Fusion {

	static Application* s_Application = nullptr;

	Application::Application(const ApplicationSpecification& InSpecification, void* InUserData)
		: m_Specification(InSpecification)
	{
		FUSION_CORE_VERIFY(s_Application == nullptr);
		s_Application = this;

		WindowSpecification WindowSpec = {};
		WindowSpec.Title = m_Specification.Title;
		WindowSpec.Width = m_Specification.WindowWidth;
		WindowSpec.Height = m_Specification.WindowHeight;
		WindowSpec.UserData = InUserData;
		m_Window = MakeUnique<Window>(WindowSpec);
		m_Window->Init();
		m_Window->SetEventCallback(FUSION_BIND_FUNC(Application::EventCallback));

		m_Context = GraphicsContext::Create();

		SwapChainInfo SwapChainCreateInfo;
		SwapChainCreateInfo.Width = m_Window->GetWidth();
		SwapChainCreateInfo.Height = m_Window->GetHeight();
		SwapChainCreateInfo.HasDepthBuffer = false;
		SwapChainCreateInfo.RenderTargetClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_SwapChain = SwapChain::Create(SwapChainCreateInfo);

		m_Window->Maximize();

		AssetLoader::RegisterDefaultLoaders();

		m_AssetStorage = Shared<AssetStorage>::Create();
	}

	Application::~Application()
	{
		m_Window = nullptr;
	}

	void Application::Run()
	{
		m_Running = true;
		OnInit();
		while (m_Running)
		{
			FrameMarkStart("MainThread");

			Keyboard::Get().TransitionHeldKeys();
			Mouse::Get().TransitionHeldButtons();
			m_Window->ProcessEvents();

			ExecuteMainThreadQueue();

			//m_Renderer->Begin();
			OnUpdate(m_TimeStep);
			//m_Renderer->End();

			TimePoint time = std::chrono::high_resolution_clock::now();
			m_FrameTime = std::chrono::duration_cast<std::chrono::duration<float>>(time - m_LastFrameTime).count();
			m_TimeStep = glm::min<float>(m_FrameTime, 0.0333f);
			m_LastFrameTime = time;
			
			m_AssetStorage->ProcessDestructionQueue();

			Mouse::Get().ResetReleasedButtons();
			Keyboard::Get().ResetReleasedKeys();

			// End of frame
			FrameMarkEnd("MainThread");

			// TODO(Peter): Handle with event
			m_Running = !m_Window->ShouldClose();
		}

		m_Context->WaitForGPU();
		OnShutdown();
	}

	Application& Application::Get() { return *s_Application; }

	void Application::EventCallback(Event& InEvent)
	{
		EventDispatcher Dispatcher(InEvent);
		Dispatcher.Dispatch<WindowCloseEvent>([this]([[maybe_unused]] WindowCloseEvent& InCloseEvent)
		{
			m_Running = false;
			return true;
		});

		Dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& InResizeEvent)
		{
			//m_SwapChain->Resize(InResizeEvent.GetWidth(), InResizeEvent.GetHeight());
			return false;
		});

		OnEvent(InEvent);
	}

	void Application::ExecuteMainThreadQueue()
	{
		std::scoped_lock<std::mutex> Lock(m_MainThreadQueueMutex);

		for (auto& Func : m_MainThreadQueue)
			Func();

		m_MainThreadQueue.clear();
	}

}
