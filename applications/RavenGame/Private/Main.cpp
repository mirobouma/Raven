#include "Engine/Engine.h"
#include "Engine/Globals.h"

#include "WindowSystem/WindowSystem.h"
#include "Engine/Windows/RenderableWindow.h"

#include "HAL/Platform.h"

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

	IPlatformWindow* Window;

	// Create window.
	if (GWindowSystem != nullptr)
	{
		Window = GWindowSystem->CreateAWindow<Raven::RRenderableWindow>(
			"Raven Game Engine <Vulkan>",
			FVector2(1024, 800),
			FVector2(0, 0),
			EPlatformWindowFlags::EPlatformWindowFlags_AutoPosition);
	}

	if (Window != nullptr)
	{
		// Show the window.
		Window->Show();
	}

	// Main loop
	while (true)
	{
		GWindowSystem->ProcessWindowMessages();

		// All windows are destroyed, therefore we should shut down.
		if (GWindowSystem->GetNumAliveWindows() == 0)
		{
			break;
		}

		GEngine->Render();

	}

	delete Window;

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



