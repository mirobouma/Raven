#include "PlatformLinux/PlatformLinuxWindow.h"
#include "SDL.h"
#undef main

namespace Raven {

	void RPlatformLinuxWindow::Show()
	{
		SDL_Window* window = SDL_CreateWindow(
			"SDL Vulkan",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			1024,
			768,
			SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	}
}