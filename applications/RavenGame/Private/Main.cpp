#include "Engine/Engine.h"
#include "Engine/Globals.h"

#include "HAL/Platform.h"
#include "HAL/PlatformApplication.h"
#include "HAL/PlatformWindow.h"

#include <chrono>
#include <thread>

using namespace Raven;
using namespace Raven::Math;

void ApplicationStart(int argc, char*argv[])
{
	// Initialize platform specific systems.
	Raven::REngine::Create();

	// Create raven application.
	Raven::RApplication App;

	// Initialize application.
	App.Initialize();

	// Create window.
	Raven::RPlatformWindow Window(
		"Raven Game Engine <Vulkan>", 
		FVector2(1024, 800), 
		FVector2(0,0), 
		EPlatformWindowFlags::EPlatformWindowFlags_AutoPosition);

	Window.Show();

	// Start application.
	App.Start();

	// Destroy Application.
	App.Destroy();

	// Destroy platform specific systems.
	Raven::REngine::Destroy();
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


