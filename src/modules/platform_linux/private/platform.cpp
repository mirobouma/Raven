#include "platform_linux/platform.h"
#include "SDL.h"

namespace Raven {

	bool RPlatformLinux::Initialize()
	{
		if (SDL_Init(SDL_INIT_VIDEO) == 0)
		{
			// Initialization succeeded, return true for successful initialization.
			return true;
		}

		// Initialization failed, return false.
		return false;
	}

	bool RPlatformLinux::Destroy()
	{
		if (SDL_WasInit(SDL_INIT_VIDEO))
		{
			// Destroy SDL.
			SDL_Quit();
		}

		return true;
	}

}