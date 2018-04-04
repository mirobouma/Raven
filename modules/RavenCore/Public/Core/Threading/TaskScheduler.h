#pragma once

#include "Core/Threading/Task.h"
#include "Core/Threading/AtomicCounter.h"
#include "Core/Threading/ThreadPool.h"

#include <vector>
#include <cinttypes>
#include <vector>
#include <functional>
#include <future>
#include <atomic>

namespace Raven 
{
	/*! Structure holding arguments for initialization of a worker thread. */
	struct FWorkerThreadCreateParams 
	{

	};

	// Task scheduler.
	class RTaskScheduler 
	{
	public:
		RTaskScheduler() {};
		~RTaskScheduler() { Destroy(); };

		// Initialize task scheduler with x number of worker threads.
		void Initialize(uint32_t numWorkers);

		// Destroy all workers,
		void Destroy();

		/*! Entry point for worker threads. */
		static void WorkerStart(RThread* thread);

		// Create a task in the scheduler.
		template <typename F, typename... Args>
		static FTask<Args...> MakeTask(F&& f, Args&&... args)
		{
			FTask<Args...> task = FTask<Args...>(std::forward<F>(f), std::forward<Args>(args)...);
			return task;
		}
		
		/*! Execute tasks and associate them with a counter. */
		void ExecuteTasks(RAtomicCounter** ppCounter, FTaskBase* pTasks, uint32_t numTasks);
	
		/*! Wait for an atomic counter to reach a certain value. */
		void WaitForCounter(RAtomicCounter* pCounter, uint32_t value);

	protected:

		/*! Initialize a worker thread. */
		static void InitializeWorker(FWorkerThreadCreateParams* args);

		std::unique_ptr<RThreadPool> _mThreadPool;
	};
}