#pragma once

#include "Core/Threading/Task.h"
#include "Core/Threading/TaskCounter.h"
#include "Core/Threading/Thread.h"
#include "Core/Threading/Fiber.h"
#include "Core/Threading/TaskAllocator.h"
#include "Core/Threading/TaskQueue.h"
#include "Core/Memory/LinearAllocator.h"

#include <vector>
#include <cinttypes>
#include <vector>
#include <functional>
#include <future>
#include <atomic>

namespace Raven 
{
	typedef enum FiberDestination {
		Unknown,
		ToPool,
		ToWaiting
	} FiberDestination;

	/*! \struct FThreadLocalStorage
	\brief Emulated thread local storage since it's hard to predict if the compiler disables TLS optimizations and this allows us to have multiple task scheduler instances. */
	struct FThreadLocalStorage 
	{
		FThreadLocalStorage() : ThreadFiber(), 
			CurrentFiberIndex(UINT32_MAX),
			OldFiberIndex(UINT32_MAX),
			OldFiberDestination(FiberDestination::Unknown),
			TaskAllocator(0),
			TaskQueue(),
			LastSuccessfulSteal(1),
			pOldFiberStoredFlag(nullptr)
		{};

		/*! Fiber that was created from this thread. Must be finished on this thread. */
		RFiber ThreadFiber;

		/*! The index of the current fiber in the fiber pool. */
		uint32_t CurrentFiberIndex;

		/*! The previously executing fiber index in the fiber pool. */
		uint32_t OldFiberIndex;

		/*! Where the fiber should go once we clean up. */
		FiberDestination OldFiberDestination;

		/*! Task allocator for this thread. */
		RTaskAllocator TaskAllocator;

		/*! The queue of waiting tasks */
		RTaskQueue<FTaskBase*> TaskQueue;

		/*! The last queue that we successfully stole from. This is an offset index from the current thread index */
		uint32_t LastSuccessfulSteal;

		/*! Pointer to the atomic flag which describes readyness of the old fiber. */
		std::atomic<bool>* pOldFiberStoredFlag;

		/*! Internal lock atomic value. */
		std::atomic_flag ReadyFibersLock;

		/*! Fibers which are ready for consumption. */
		std::vector<std::pair<std::size_t, std::atomic<bool> *>> ReadyFibers;

	private:
		/*! Cache-line padding to prevent false sharing. */
		char pad[64];
	};

	/*! \struct FWorkerStartArgs
	\brief Arguments for the ThreadStart function. */
	struct FWorkerStartArgs {
		RTaskScheduler* scheduler;
		uint32_t threadId;
	};

	/*! \struct FMainFiberArgs
	\brief Arguments for the MainFiberStart function. */
	struct FMainFiberArgs {
		RTaskScheduler* scheduler;
		FTaskBase* task;
	};

	// Task scheduler.
	class RTaskScheduler 
	{
	public:
		RTaskScheduler() {};
		~RTaskScheduler() { Destroy(); };

		// Initialize task scheduler with x number of worker threads.
		void Run(uint32_t numWorkers, uint32_t numFibers, FTaskBase* mainTask);

		// Destroy all workers,
		void Destroy();

		// Create a task in the scheduler.
		template <typename F, typename... Args>
		FTask<Args...>* MakeTask(F&& f, Args... args)
		{
			return _mTLS[GetCurrentThreadIndex()].TaskAllocator.MakeTask(std::forward<F>(f), std::forward<Args>(args)...);
		}

		/*! Execute a singular task and associate it with a counter. */
		void ExecuteTask(RTaskCounter* pCounter, FTaskBase* pTask);

		/*! Execute tasks and associate them with a counter. */
		void ExecuteTasks(RTaskCounter* pCounter, FTaskBase** ppTasks, uint32_t numTasks);
	
		/*! Wait for an atomic counter to reach a certain value. */
		void WaitForCounter(RTaskCounter* pCounter, uint32_t value = 0);
		
		/*! Reset any allocated tasks. (Should be called at the end of each frame (after waiting for all counters). */
		void ResetTaskAllocations();

		/*! Get amount of threads. */
		uint32_t GetNumThreads() const { return _mNumThreads; }

		/*! Get thread index for the currently executing thread. */
		inline uint32_t GetCurrentThreadIndex() const 
		{
			for (uint32_t i = 0; i < _mNumThreads; i++)
			{
				if (_mThreads[i]->IsCurrentThread())
					return i;
			}

			return UINT32_MAX;
		}
		/*! Get the index of a fiber which is free to be used. */
		uint32_t GetFreeFiberIndex();

	protected:
		friend class RTaskCounter;

		/*! Retrieves the next task from the task queue. 
		\returns false if there is no task available */
		bool GetNextTask(FTaskBase** nextTask);

		/*! If necessary, moves the old fiber to the fiber pool or the waiting list */
		void CleanUpFiber();

		/*! Marks a fiber as ready for executioh since it's associating counter value has been reached. */
		void AddReadyFiber(uint32_t fiberIndex, std::atomic<bool> *fiberStoredFlag);

		/*! Entry point for the worker threads. */
		static void ThreadStart(void* arg);

		/*! Entry point for thread 0. */
		static void MainFiberStart(void* arg);

		/*! Entry point for fibers. */
		static void FiberStart(void* arg);

		/*! Atomic booleans to check if the task scheduler has been fully initialized and if it should shut down any worker threads. */
		std::atomic<bool> _mInitialized;
		std::atomic<bool> _mShouldQuit;
		std::atomic<bool> _mFinishedCleanup;

		uint32_t _mNumFibers; /*!< Amount of fibers allocated for this task scheduler. */
		RFiber* _mFibers; /*!< Array of fibers. */
		std::atomic<bool> *_mFreeFibers; /*! Array of atomic boolean to signify if a fiber is free for usage. */

		uint32_t _mNumThreads; /*!< Amount of worker threads in this task scheduler. */
		std::vector<RThread*> _mThreads; /*!< Thread pool for this task scheduler. */
		FThreadLocalStorage* _mTLS; /*!< Array of thread local storage objects (one for each thread.) */

	};

	extern RTaskScheduler* GTaskScheduler;
}