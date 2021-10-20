#include "stdafx.hpp"
#include "System.h"

#include <windows.h>

core::System::System(BaseApplication* app)
	: m_app(app)
{
	m_app->init();
}

void core::System::run()
{
	MSG msg;
	bool done, result;

	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user.
	done = false;
	while (!done)
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
			{
				done = true;
			}
		}
		else
		{
			// If windows signals to end the application then exit out.
			//if (msg.message == WM_QUIT)
			//{
			//	done = true;
			//}
			//else
			//{
			// Otherwise do the frame processing.

			m_timer.frame();
			result = m_app->update(m_timer.getTime());
			if (!result)
			{
				done = true;
				break;
			}
			result = m_app->render();
			if (!result)
			{
				done = true;
				break;
			}

			//}
		}
	}
	PostQuitMessage(0);
	return;
}