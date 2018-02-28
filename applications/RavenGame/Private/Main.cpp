#include "Engine/Engine.h"
#include "Engine/Globals.h"

#include "HAL/Platform.h"
#include "HAL/PlatformApplication.h"
#include "HAL/PlatformWindow.h"

void ApplicationStart(int argc, char*argv[])
{
	// Initialize platform specific systems.
	Raven::RPlatform::Initialize();

	// Create raven application.
	Raven::RApplication App;
	App.Initialize();

	// Create window.
	Raven::RPlatformWindow Window;
	Window.Show();

	// Start application.
	App.Start();

	// Destroy Application.
	App.Destroy();

	// Destroy platform specific systems.
	Raven::RPlatform::Destroy();
}

#ifdef PLATFORM_WINDOWS
	int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
	{
		ApplicationStart(0, nullptr);
		return 0;
	}

#else
	int main(int argc, char *argv[])
	{
		ApplicationStart(argc, argv);
		return 0;
	}
#endif


