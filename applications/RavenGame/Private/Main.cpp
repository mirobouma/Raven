#include "Engine/Engine.h"
#include "Engine/Globals.h"

#include "WindowSystem/WindowSystem.h"
#include "Engine/Windows/RenderableWindow.h"
#include "Core/Threading/TaskScheduler.h"

#include "HAL/Platform.h"

#include "RenderingVulkan/TestVulkanRenderer.h"
#include "Core/Debug/LogManager.h"
#include "Core/Memory/BuddyAllocator.h"
#include <chrono>
#include <thread>

#ifdef PLATFORM_WINDOWS
#ifndef NDEBUG
#pragma comment(linker, "/SUBSYSTEM:CONSOLE")
#endif
#endif

using namespace Raven;
using namespace Raven::Math;

RTaskScheduler* gTaskScheduler = nullptr;


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
	GEngine->InitializeSystems();

	IPlatformWindow* Window;


	RTaskCounter counter;
	FTaskBase* task = gTaskScheduler->MakeTask([]()
	{
		RTaskCounter counter2;
		FTaskBase* task2[10];

		for (uint32_t x = 0; x < 10; ++x)
		{
			task2[x] = gTaskScheduler->MakeTask([]()
			{
				RTaskCounter counter3;
				FTaskBase* task3[5];

				for (uint32_t z = 0; z < 5; ++z)
				{
					task3[z] = gTaskScheduler->MakeTask([](uint32_t i)
					{
						RLOG_INFO("wazzaaaZaaaaaZazazaza %i", i);
					}, z);
				}

				gTaskScheduler->ExecuteTasks(&counter3, &task3[0], 5);
				gTaskScheduler->WaitForCounter(&counter3, 0);

				RLOG_INFO("wazzaaaZaaaaa");

			});
		}

		gTaskScheduler->ExecuteTasks(&counter2, &task2[0], 10);
		gTaskScheduler->WaitForCounter(&counter2, 0);

		RLOG_INFO("wazzaaa");
	});

	gTaskScheduler->ExecuteTasks(&counter, &task, 1);
	gTaskScheduler->WaitForCounter(&counter, 0);
	RLOG_INFO("wazzaaadone");

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

	RBuddyAllocator balloc(1022);
	
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
		RTaskAllocator taskAllocator;
		auto task = taskAllocator.MakeTask([=](uint32_t i) { ApplicationStart(0, nullptr); }, 0);

		RTaskScheduler scheduler;
		scheduler.Run(1, 60, task);

		taskAllocator.Reset(true);
		//ApplicationStart(0, nullptr);
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
	
	gTaskScheduler = new RTaskScheduler();
	gTaskScheduler->Run(0, 160, &task);
	
	taskAllocator.Reset(true);
	//ApplicationStart(0, nullptr);
	return 0;
}



