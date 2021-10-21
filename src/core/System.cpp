#include "stdafx.hpp"
#include "System.h"

#include <windows.h>

void core::System::Run(BaseApplication* app)
{
	MSG msg;
	bool done, result;

	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	done = !app->Init();

	// Loop until there is a quit message from the window or the user.
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
			m_timer.frame();
			result = app->Update(m_timer.getTime());
			if (!result)
			{
				done = true;
				break;
			}
			result = app->Render();
			if (!result)
			{
				done = true;
				break;
			}

			//}
		}
	}

	app->Release();

	PostQuitMessage(0);
	return;
}