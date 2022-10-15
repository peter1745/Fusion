#pragma once

#include "Fusion/Core/Window.h"

namespace Fusion {

	class WindowsWindow : public Window
	{
	public:
		using MessageProcFunc = std::function<LRESULT(HWND hwnd, UINT uMessage, WPARAM wParam, LPARAM lParam)>;

		WindowsWindow(const WindowSpecification& InSpecification);

		virtual uint32_t GetWidth() const override { return 0; }
		virtual uint32_t GetHeight() const override { return 0; }

		virtual void SetEventCallback(const EventCallbackFunc& InCallbackFunc) { m_EventCallbackFunc = InCallbackFunc; }

		virtual void* GetWindowHandle() const override { return static_cast<void*>(m_WindowHandle); }
		virtual bool ShouldClose() const override { return m_Closed; }

		void RegisterMessageProc(const MessageProcFunc& InFunc) { m_MessageProcFunc = InFunc; }
		const MessageProcFunc& GetMessageProcFunc() const { return m_MessageProcFunc; }
	private:
		virtual void Init() override;
		virtual void ProcessEvents() override;

		void SendEvent(Event& InEvent) const;

	private:
		friend LRESULT CALLBACK MessageHandler(HWND hwnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

	private:
		WindowSpecification m_Specification;
		HWND m_WindowHandle;
		HDC m_HDC;
		int m_PixelFormatIndex;

		EventCallbackFunc m_EventCallbackFunc = nullptr;
		MessageProcFunc m_MessageProcFunc = nullptr;

		bool m_Closed = false;
	};

}
