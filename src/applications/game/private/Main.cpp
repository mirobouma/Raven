#include "engine/engine.h"
#include "engine/globals.h"

#include "core/window_system/window_system.h"
#include "engine/windows/renderable_window.h"
#include "core/threading/task_scheduler.h"
#include "core/debug/profiler.h"
#include "hal/platform.h"
#include "core/debug/log_manager.h"
#include "core/memory/buddy_allocator.h"
#include "game_window.h"
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
		Window = GWindowSystem->CreateWindow<Raven::RGameWindow>(
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
			//PROFILER_START_SCOPE("Make Tasks");
			//
			//	std::vector<RTaskCounter> counters(20);
			//
			//	for (uint32_t i = 0; i < 20; ++i)
			//	{
			//		auto t = GTaskScheduler->MakeTask([=] {
			//			PROFILER_START_SCOPE("Sleep first");
			//
			//
			//			auto t2 = GTaskScheduler->MakeTask([=] {
			//				PROFILER_START_SCOPE("Sleep second");
			//
			//			});
			//
			//			RTaskCounter counter;
			//			GTaskScheduler->ExecuteTask(&counter, t2);
			//			GTaskScheduler->WaitForCounter(&counter);
			//
			//		});
			//
			//		GTaskScheduler->ExecuteTask(&counters[i], t);
			//	}
			//
			//	PROFILER_START_SCOPE("Wait for all tasks");
			//	for (uint32_t i = 0; i < 20; ++i)
			//	{
			//		GTaskScheduler->WaitForCounter(&counters[i]);
			//	}
			//	PROFILER_END_SCOPE

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
		GTaskScheduler->Run(1, 60, &task);

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
	GTaskScheduler->Run(1, 60, &task);
	
	taskAllocator.Reset(true);
	delete GTaskScheduler;
	//ApplicationStart(0, nullptr);
	return 0;
}



