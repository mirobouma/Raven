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

void ApplicationStart(int argc, char*argv[])
{
	// Initialize platform specific systems.
	REngine::Create();
	
	if (GEngine == nullptr)
	{
		// Initialization of engine fail, end application.
		return;
	}

#ifdef ENABLE_PROFILING
	RProfiler::Initialize();
#endif
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
		PROFILER_BEGIN
		{
			PROFILER_START_SCOPE("Process Windows Messages");
			GWindowSystem->ProcessWindowMessages();
			
			// All windows are destroyed, therefore we should shut down.
			if (GWindowSystem->GetNumAliveWindows() == 0)
			{
				break;
			}
			PROFILER_END_SCOPE
			PROFILER_START_SCOPE("Make Tasks");
			
				std::vector<RTaskCounter> counters(20);
			
				for (uint32_t i = 0; i < 20; ++i)
				{
					auto t = GTaskScheduler->MakeTask([=] {
						PROFILER_START_SCOPE("Sleep first");
			
			
						auto t2 = GTaskScheduler->MakeTask([=] {
							PROFILER_START_SCOPE("Sleep second");
			
						});
			
						RTaskCounter counter;
						GTaskScheduler->ExecuteTask(&counter, t2);
						GTaskScheduler->WaitForCounter(&counter);
			
					});
			
					GTaskScheduler->ExecuteTask(&counters[i], t);
				}
			
				PROFILER_START_SCOPE("Wait for all tasks");
				for (uint32_t i = 0; i < 20; ++i)
				{
					GTaskScheduler->WaitForCounter(&counters[i]);
				}
				PROFILER_END_SCOPE

			PROFILER_START_SCOPE("Render Window");
			if (Window != nullptr)
			{
				Window->Render();
			}
			PROFILER_END_SCOPE

			GEngine->Render();
		}
			PROFILER_END

			GTaskScheduler->ResetTaskAllocations();
	}

	delete Window;

#ifdef ENABLE_PROFILING
	RProfiler::Destroy();
#endif

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
		GTaskScheduler->Run(6, 60, &task);

		taskAllocator.Reset(true);
		delete GTaskScheduler;
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
	GTaskScheduler->Run(6, 60, &task);
	
	taskAllocator.Reset(true);
	delete GTaskScheduler;
	//ApplicationStart(0, nullptr);
	return 0;
}



