#include "Engine/Engine.h"
#include "Engine/Globals.h"

#include "HAL/Platform.h"
#include "HAL/PlatformApplication.h"
#include "HAL/PlatformWindow.h"

#include "RenderingVulkan/TestVulkanRenderer.h"

#include <chrono>
#include <thread>

#ifdef PLATFORM_WINDOWS
#ifndef NDEBUG
#pragma comment(linker, "/SUBSYSTEM:CONSOLE")
#endif
#endif

using namespace Raven;
using namespace Raven::Math;

void ApplicationStart(int argc, char*argv[])
{
	// Initialize platform specific systems.
	REngine::Create();
	
	if (GEngine == nullptr)
	{
		// Initialization of engine fail, end application.
		return;
	}

	// Initialize engine systems.
	GEngine->InitializeMinimal();
	GEngine->InitializeSystems();

	// Create raven application.
	RApplication App;

	// Initialize application.
	App.Initialize();

	// Create window.
	Raven::RPlatformWindow Window(
		"Raven Game Engine <Vulkan>", 
		FVector2(1024, 800), 
		FVector2(0,0), 
		EPlatformWindowFlags::EPlatformWindowFlags_AutoPosition);

	// Create Vulkan surface for this window.
	GEngine->GetRenderer()->CreateSurface((void*)Window.GetNativeHandle());
    GEngine->GetRenderer()->CreateSwapchain();

	// Show the window.
	Window.Show();

	// Main loop
	while (true)
	{
		App.Update();
		GEngine->Render();

		if (Window.ShouldQuit())
		{
			break;
		}
	}

	// Destroy Application.
	App.Destroy();

	// Destroy platform specific systems.
	REngine::Destroy();
}

#ifdef PLATFORM_WINDOWS
	int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
	{
		ApplicationStart(0, nullptr);
		return 0;
	}
#endif

int main(int argc, char *argv[])
{
	ApplicationStart(argc, argv);
	return 0;
}



