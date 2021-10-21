#include "stdafx.hpp"
#include "Window.h"
#include "Input.h"

core::Window::Window(Input* in, const WindowConfig& config)
	: m_input(in)
{
	ApplicationHandle = this;

	WNDCLASSEX wc;
	int posX = 0;
	int posY = 0;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(nullptr);

	// Give the application a name.
	m_applicationName = config.window_name;

	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	DWORD dwExStyle = WS_EX_APPWINDOW;
	DWORD dwStyle = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME;
	m_width = config.width;
	m_height = config.height;

	if (m_width == 0 || m_height == 0)
	{
		m_width = GetSystemMetrics(SM_CXSCREEN);
		m_height = GetSystemMetrics(SM_CYSCREEN);
	}

	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(dwExStyle, m_applicationName, nullptr, dwStyle, posX, posY, m_width, m_height, nullptr, nullptr, m_hinstance, nullptr);

	if (config.borderless)
	{
		SetWindowLong(m_hwnd, GWL_STYLE, 0); //remove all window styles, check MSDN for details
	}

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);
}

core::Window::~Window()
{
	ApplicationHandle = nullptr;

	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	//if (FULL_SCREEN)
	//{
	//	ChangeDisplaySettings(NULL, 0);
	//}

	// Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = nullptr;

	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = nullptr;
}

LRESULT CALLBACK core::Window::messageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK core::Window::WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// Handle keyboard and mouse input.
	case WM_KEYDOWN:
	{
		ApplicationHandle->m_input->SetKeyDown(wparam);
		break;
	}
	case WM_KEYUP:
	{
		ApplicationHandle->m_input->SetKeyUp(wparam);
		break;
	}
	case WM_MOUSEMOVE:
	{
		ApplicationHandle->m_input->setMouseX(LOWORD(lparam));
		ApplicationHandle->m_input->setMouseY(HIWORD(lparam));
		break;
	}
	case WM_LBUTTONDOWN:
	{
		ApplicationHandle->m_input->setLeftMouse(true);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		ApplicationHandle->m_input->setRightMouse(true);
		break;
	}
	case WM_LBUTTONUP:
	{
		ApplicationHandle->m_input->setLeftMouse(false);
		break;
	}
	case WM_RBUTTONUP:
	{
		ApplicationHandle->m_input->setRightMouse(false);
		break;
	}
	// Check if the window is being destroyed.
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	// Check if the window is being closed.
	case WM_CLOSE:
	{
		DestroyWindow(hwnd);
		PostQuitMessage(0);
		return 0;
	}
	}

	//if (ImGui_ImplDX11_WndProcHandler(hwnd, umessage, wparam, lparam))
	//	return true;
	if (ImGui_ImplWin32_WndProcHandler(hwnd, umessage, wparam, lparam))
		return true;

	return ApplicationHandle->messageHandler(hwnd, umessage, wparam, lparam);
}