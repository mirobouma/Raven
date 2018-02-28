#include "PlatformLinux/PlatformLinuxApplication.h"
#include "SDL.h"
#undef main 

namespace Raven {

	bool RPlatformLinuxApplication::Initialize()
	{
		// Initialization succeeded, return true.
		return false;
	}

	bool RPlatformLinuxApplication::Destroy()
	{
		// Deinitialization succeeded, return true.
		return true;
	}


	void RPlatformLinuxApplication::Start()
	{
		SDL_Event event;

		while (true)
		{
			while (SDL_PollEvent(&event)) {
				switch (event.type)
				{
				case SDL_QUIT:
					return;
				}
			}
		}
	}

}