#include "PlatformWindows/PlatformWindowsApplication.h"
#include "PlatformWindows/PlatformWindows.h"

namespace Raven {

	bool RPlatformWindowsApplication::Initialize()
	{
		// Initialization succeeded, return true.
		return false;
	}

	bool RPlatformWindowsApplication::Destroy()
	{
		// Deinitialization succeeded, return true.
		return true;
	}

	void RPlatformWindowsApplication::Start()
	{
		// this struct holds Windows event messages
		MSG msg;

		// Game Loop
		while (true)
		{
			// Check to see if any messages are waiting in the queue
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				// translate keystroke messages into the right format
				TranslateMessage(&msg);

				// send the message to the WindowProc function
				DispatchMessage(&msg);

				// check to see if it's time to quit
				if (msg.message == WM_QUIT)
					break;
			}

			if (msg.message == WM_QUIT)
				break;
		}
	}

}