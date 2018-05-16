#include "Engine/Engine.h"
#include "Engine/Globals.h"

#include "WindowSystem/WindowSystem.h"
#include "Engine/Windows/RenderableWindow.h"
#include "Core/Threading/TaskScheduler.h"
#include "Core/Debug/Profiler.h"

#include "HAL/Platform.h"

#include "RenderingVulkan/TestVulkanRenderer.h"
#include "Core/Debug/LogManager.h"
#include "Core/Memory/BuddyAllocator.h"
#include "GameWindow.h"
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

	RProfiler::Initialize();

	// Initialize engine systems.
	GEngine->InitializeSystems();

	RGameWindow* Window;

	// Create window.
	if (GWindowSystem != nullptr)
	{
		Window = GWindowSystem->CreateAWindow<Raven::RGameWindow>(
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

		if(Window != nullptr)
		{
			Window->Render();
		}

		GEngine->Render();

	}

	delete Window;

	RProfiler::Destroy();

	// Destroy platform specific systems.
	REngine::Destroy();

}

#ifdef PLATFORM_WINDOWS
	int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
	{
		RTaskAllocator taskAllocator;
		auto task = FTask<uint32_t>([=](uint32_t i)
		{
			ApplicationStart(0, nullptr);
		}, 0);

		GTaskScheduler = new RTaskScheduler();
		GTaskScheduler->Run(0, 60, &task);

		taskAllocator.Reset(true);
		return 0;
	}
#endif

int main(int argc, char *argv[])
{
	RTaskAllocator taskAllocator;
	auto task = FTask<uint32_t>([=](uint32_t i)
	{
		ApplicationStart(0, nullptr);
	}, 0);
	
	GTaskScheduler = new RTaskScheduler();
	GTaskScheduler->Run(4, 60, &task);
	
	taskAllocator.Reset(true);
	delete GTaskScheduler;
	//ApplicationStart(0, nullptr);
	return 0;
}



