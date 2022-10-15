#include "FusionPCH.h"
#include "WindowsWindow.h"
#include "Fusion/Events/WindowEvents.h"

namespace Fusion {

	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
	{
		WindowsWindow* This = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

		if (This && This->m_MessageProcFunc && This->m_MessageProcFunc(hwnd, uMessage, wParam, lParam))
			return TRUE;

		switch (uMessage)
		{
		case WM_SIZE:
		{
			This->m_Specification.Width = LOWORD(lParam);
			This->m_Specification.Height = HIWORD(lParam);

			WindowResizeEvent ResizeEvent(LOWORD(lParam), HIWORD(lParam));
			This->SendEvent(ResizeEvent);
			break;
		}
		case WM_CLOSE:
		case WM_DESTROY:
		{
			WindowCloseEvent CloseEvent;
			This->SendEvent(CloseEvent);
			PostQuitMessage(0);
			break;
		}
		}

		return DefWindowProc(hwnd, uMessage, wParam, lParam);
	}

	WindowsWindow::WindowsWindow(const WindowSpecification& InSpecification)
		: m_Specification(InSpecification)
	{
	}

	void WindowsWindow::Init()
	{
		WNDCLASS WindowClass;
		ZeroMemory(&WindowClass, sizeof(WNDCLASS));

		// GetModuleHandle should be safe since we're doing static linking
		WindowClass.hInstance = GetModuleHandle(NULL);
		WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		WindowClass.lpszClassName = L"Fusion Window Class";
		WindowClass.lpfnWndProc = MessageHandler;

		RegisterClass(&WindowClass);

		int32_t Width = static_cast<int32_t>(m_Specification.Width);
		int32_t Height = static_cast<int32_t>(m_Specification.Height);
		int32_t CenterX = (GetSystemMetrics(SM_CXSCREEN) - Width) / 2;
		int32_t CenterY = (GetSystemMetrics(SM_CYSCREEN) - Height) / 2;

		HWND WindowHandle = CreateWindowEx(
			NULL,
			L"Fusion Window Class",
			L"My Title",
			WS_OVERLAPPEDWINDOW,
			CenterX, CenterY,
			Width, Height,
			NULL,
			NULL,
			WindowClass.hInstance,
			NULL
		);

		FUSION_CORE_VERIFY(WindowHandle != NULL, "Failed to create Win32 window!");

		SetWindowLongPtr(WindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		m_HDC = GetDC(WindowHandle);
		FUSION_CORE_VERIFY(m_HDC != NULL, "Failed to get window HDC");

		PIXELFORMATDESCRIPTOR PixelDescriptor;
		ZeroMemory(&PixelDescriptor, sizeof(PIXELFORMATDESCRIPTOR));
		PixelDescriptor.nSize = sizeof(PixelDescriptor);
		PixelDescriptor.nVersion = 1;
		PixelDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
		PixelDescriptor.iPixelType = PFD_TYPE_RGBA;
		PixelDescriptor.cColorBits = 32;
		PixelDescriptor.cDepthBits = 16;
		PixelDescriptor.cStencilBits = 0;
		PixelDescriptor.cAuxBuffers = 0;
		PixelDescriptor.iLayerType = PFD_MAIN_PLANE;

		m_PixelFormatIndex = ChoosePixelFormat(m_HDC, &PixelDescriptor);
		FUSION_CORE_VERIFY(m_PixelFormatIndex, "Failed to pick appropriate pixel format!");
		SetPixelFormat(m_HDC, m_PixelFormatIndex, &PixelDescriptor);

		m_WindowHandle = WindowHandle;

		ShowWindow(m_WindowHandle, TRUE);
	}

	void WindowsWindow::ProcessEvents()
	{
		MSG Message = {};
		while (PeekMessage(&Message, NULL, NULL, NULL, PM_REMOVE) > 0)
		{
			if (Message.message == WM_QUIT)
			{
				m_Closed = true;
				return;
			}

			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
	}

	void WindowsWindow::SendEvent(Event& InEvent) const
	{
		if (!m_EventCallbackFunc)
			return;

		m_EventCallbackFunc(InEvent);
	}

}
