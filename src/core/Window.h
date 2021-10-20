#pragma once

#include <windows.h>
#include "Input.h"

namespace core
{
	class Window
	{
	public:
		Window(Input* in);
		~Window();

		struct WindowConfig
		{
			const wchar_t* window_name = L"Cheeseboard";
			bool borderless = false;

			// leaving these 0 will automaticaly find screen size
			int width = 0;
			int height = 0;
		};

		void init(const WindowConfig& config);
		void release();

		HWND hwnd() { return m_hwnd; }
		int screen_width() { return m_width; }
		int screen_height() { return m_height; }

	private:

		bool m_initialised = false;

		int m_width = 0;
		int m_height = 0;

		LRESULT CALLBACK messageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);
		static LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
		core::Input* m_input = nullptr;
		HWND m_hwnd = nullptr;
		LPCWSTR m_applicationName = nullptr;
		HINSTANCE m_hinstance = nullptr;
	};
	static Window* ApplicationHandle = nullptr;
}
