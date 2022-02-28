#include "stdafx.hpp"
#include "System.h"

#include <windows.h>
#include "core/Logging.h"

void core::System::Run(BaseApplication* app)
{
	MSG msg;
	bool done, result;

	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	CB_LOG_INIT;

	done = !app->Init();
	CB_LOG("Initialization Complete");

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
			m_updateTimer.frame();
			result = app->Update(m_updateTimer.getTime());
			if (!result)
			{
				done = true;
				break;
			}

			// allows app to disable rendering for a frame
			if (app->ReadyForRender())
			{
				m_renderTimer.frame();
				result = app->Render(m_renderTimer.getTime());
				if (!result)
				{
					done = true;
					CB_LOG_FRAME;
					break;
				}
				CB_LOG_FRAME;
			}
		}
	}

	app->Release();
	CB_LOG("Release Complete");

	CB_LOG_FRAME;
	CB_LOG_RELEASE;

	PostQuitMessage(0);
	return;
}