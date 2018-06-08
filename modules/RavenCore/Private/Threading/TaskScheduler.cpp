#include "Core/Threading/TaskScheduler.h"
#include "HAL/PlatformInfo.h"
#include "Core/Debug/LogManager.h"
#include "Core/Debug/Profiler.h"

// Task scheduler based on https://github.com/RichieSams/FiberTaskingLib/

namespace Raven 
{
	RTaskScheduler* GTaskScheduler = nullptr;

	void RTaskScheduler::Run(uint32_t numThreads, uint32_t numFibers, FTaskBase* mainTask)
	{
		RASSERT(numThreads <= numFibers);

		// We haven't completely initialized yet and we should not quit.
		_mInitialized.store(false, std::memory_order::memory_order_release);
		_mShouldQuit.store(false, std::memory_order_release);
		_mFinishedCleanup.store(false, std::memory_order_release);

		_mNumFibers = numFibers;

		// Allocate memory for fibers.
		_mFibers = (RFiber*)malloc(sizeof(RFiber) * numFibers);
		_mFreeFibers = new std::atomic<bool>[numFibers];

		// Create fiber instances.
		for (uint32_t i = 0; i < _mNumFibers; i++)
		{

			new (&_mFibers[i]) RFiber(FiberStart, this, (size_t)512000);
			_mFreeFibers[i].store(true, std::memory_order_release);
		}

		if (numThreads == 0) {
			// 1 thread for each logical processor
			_mNumThreads = RPlatformInfo::GetHardwareConcurrency();
		}
		else {
			_mNumThreads = numThreads;
		}

		// Allocate memory for fibers (set them to null objects).
		_mThreads = std::vector<RThread*>(_mNumThreads);

		// Allocate thread-local storage.
		_mTLS = new FThreadLocalStorage[_mNumThreads];

		// Initialize all the locks before we start the other threads
		for (uint32_t i = 0; i < _mNumThreads; ++i) {
			_mTLS[i].ReadyFibersLock.clear();
		}

		// Set the first thread to be the current thread, and set it's affinity.
		_mThreads[0] = new RThread();
		_mThreads[0]->FromCurrentThread();
		_mThreads[0]->SetAffinity(0);

		// Create worker threads.
		for (uint32_t i = 1; i < _mNumThreads; ++i)
		{
			FWorkerStartArgs* args = (FWorkerStartArgs*)malloc(sizeof(FWorkerStartArgs));
			args->scheduler = this;
			args->threadId = i;

			// Create threads and set their affinity.
			_mThreads[i] = new RThread(ThreadStart, (void*)args, i);
		}

		// Signal the worker threads that we're fully initialized
		_mInitialized.store(true, std::memory_order_release);


		// Get a free fiber 
		uint32_t freeFiberIndex = UINT32_MAX;

		// Keep on trying to acquire a free fiber (if fiber pool < num threads this could possibly result in a dead lock.
		while (freeFiberIndex == UINT32_MAX)
		{
			freeFiberIndex = GetFreeFiberIndex();
		}

		// When all threads are initialized, we can safely covert this thread into a fiber and switch to the first task.
		_mTLS[0].CurrentFiberIndex = freeFiberIndex;

		FMainFiberArgs* args = (FMainFiberArgs*)malloc(sizeof(FMainFiberArgs));
		args->scheduler = this;
		args->task = mainTask;

		// Make this fiber start at the main fiber function.
		_mFibers[freeFiberIndex].Reset(MainFiberStart, args);

		// Convert this thread into a fiber and switch to one of the available fibers.
		_mTLS[0].ThreadFiber.SwitchToFiber(&_mFibers[freeFiberIndex]);

		// We switched back to the main fiber, which means we should shut down.
		for (std::size_t i = 1; i < _mNumThreads; ++i) {
			_mThreads[i]->Join();
		}

		// free memory allocated for fibers.
		for (std::size_t i = 0; i < _mNumFibers; ++i) 
		{
			_mFibers[i].~RFiber();
		}

		free(_mFibers);
		delete[] _mFreeFibers;

		for (std::size_t i = 0; i < _mNumThreads; ++i)
		{
			delete _mThreads[i];
		}

		delete[] _mTLS;

		_mFinishedCleanup.store(true, std::memory_order_release);
	}

	void RTaskScheduler::ResetTaskAllocations()
	{
		for (uint32_t i = 0; i < _mNumThreads; i++)
		{
			_mTLS[i].TaskAllocator.Reset(false);
		}
	}

	void RTaskScheduler::Destroy()
	{
		// Wait for the task scheduler to be destroyed.
		while (!_mFinishedCleanup.load(std::memory_order_acquire)) {
			continue; // spin
		}
	}

	void RTaskScheduler::ExecuteTask(RTaskCounter* pCounter, FTaskBase* pTask)
	{
		// Call executetasks with a singular task.
		ExecuteTasks(pCounter, &pTask, 1);
	}

	void RTaskScheduler::ExecuteTasks(RTaskCounter* pCounter, FTaskBase** ppTasks, uint32_t numTasks)
	{
		// Set the counter value to the number of tasks.
		if (pCounter != nullptr) 
		{
			pCounter->_mTaskScheduler = this;
			pCounter->Store(numTasks);
		}

		// Get thread local storage.
		FThreadLocalStorage &tls = _mTLS[GetCurrentThreadIndex()];

		for (uint32_t i = 0; i < numTasks; ++i) 
		{
			// Set counter for this task.
			ppTasks[i]->_mCounter = pCounter;

			// Add task to the queue of this thread.
			tls.TaskQueue.Push(ppTasks[i]);
		}
	}

	void RTaskScheduler::WaitForCounter(RTaskCounter* pCounter, uint32_t value)
	{
		// If we already reached the value, we can return immediately.
		if (pCounter->Load(std::memory_order_relaxed) == value) {
			return;
		}

		// Get TLS and current fiber index for this thread.
		FThreadLocalStorage &tls = _mTLS[GetCurrentThreadIndex()];
		uint32_t currentFiberIndex = tls.CurrentFiberIndex;

		// Set up a flag for when a fiber is free for
		std::atomic<bool> *fiberStoredFlag = new std::atomic<bool>(false);
		bool alreadyDone = pCounter->AddFiberToWaitingList(tls.CurrentFiberIndex, value, fiberStoredFlag);

		// The counter finished while we were trying to put it in the waiting list.
		if (alreadyDone) {
			delete fiberStoredFlag;
			return;
		}

		// Get a free fiber
		uint32_t freeFiberIndex = GetFreeFiberIndex();

		// Fill in tls
		tls.OldFiberIndex = currentFiberIndex;
		tls.CurrentFiberIndex = freeFiberIndex;
		tls.OldFiberDestination = FiberDestination::ToWaiting;
		tls.pOldFiberStoredFlag = fiberStoredFlag;
		

#ifdef ENABLE_PROFILING
		bool gotProfilerContext = false;
		FProfilerScopeData data;

		if (GProfiler != nullptr)
		{
			FThreadProfiler* prof = GProfiler->GetThreadProfiler();
			if (prof != nullptr && prof->inScope)
			{
				data = prof->currentScope;
				gotProfilerContext = true;
			}
		}
#endif
		PROFILER_END_SCOPE
		PROFILER_START_SCOPE("Switch Fiber");
		// Switch to a new fiber.
		_mFibers[currentFiberIndex].SwitchToFiber(&_mFibers[freeFiberIndex]);
		PROFILER_END_SCOPE
#ifdef ENABLE_PROFILING
		if (gotProfilerContext)
		{
			PROFILER_START_SCOPE(data._name);
		}
#endif

		// We switched back to this context, therefore we need to clean up the old fiber.
		CleanUpFiber();

	}

	void RTaskScheduler::ThreadStart(void* arg)
	{
		// Get arguments for this function.
		FWorkerStartArgs* args = static_cast<FWorkerStartArgs*>(arg);
		RTaskScheduler* scheduler = args->scheduler;
		uint32_t threadId = args->threadId;

		// We retrieved our variables from the arguments to we can safely free the argument object.
		free(args);

		// Wait for the task scheduler to be initialized.
		while (!scheduler->_mInitialized.load(std::memory_order_acquire)) {
			continue; // spin
		}

		// Get next free fiber.
		uint32_t freeFiberIndex = UINT32_MAX;

		freeFiberIndex = scheduler->GetFreeFiberIndex();

		// We should always get a free fiber.
		RASSERT(freeFiberIndex != UINT32_MAX);

		// When all threads are initialized, we can safely covert this thread into a fiber and switch to the first task.
		scheduler->_mTLS[threadId].CurrentFiberIndex = freeFiberIndex;

		// Convert this thread into a fiber and switch to one of the available fibers.
		scheduler->_mTLS[threadId].ThreadFiber.SwitchToFiber(&scheduler->_mFibers[freeFiberIndex]);

		// we finished the thread if we reach this point.
	}

	void RTaskScheduler::MainFiberStart(void* arg)
	{
		// Get arguments for this function.
		FMainFiberArgs* args = static_cast<FMainFiberArgs*>(arg);
		RTaskScheduler* scheduler = args->scheduler;
		FTaskBase* task = args->task;

		// We retrieved our variables from the arguments to we can safely free the argument object.
		free(args);

		// Execute the main task.
		task->Execute();

		// Post quit message.
		scheduler->_mShouldQuit.store(true, std::memory_order_release);
		
		// Switch to the thread fiber
		FThreadLocalStorage &tls = scheduler->_mTLS[scheduler->GetCurrentThreadIndex()];
		scheduler->_mFibers[tls.CurrentFiberIndex].SwitchToFiber(&tls.ThreadFiber);
	}

	void RTaskScheduler::FiberStart(void* arg)
	{
		RTaskScheduler* scheduler = (RTaskScheduler*)arg;

		// Clean up this fiber for execution.
		scheduler->CleanUpFiber();

		// Fiber loop.
		while (!scheduler->_mShouldQuit.load(std::memory_order_acquire)) 
		{
			// Check if there are any ready fibers
			uint32_t waitingFiberIndex = UINT32_MAX;
			FThreadLocalStorage &tls = scheduler->_mTLS[scheduler->GetCurrentThreadIndex()];

			// Lock
			while (tls.ReadyFibersLock.test_and_set(std::memory_order_acquire)) {
				// Spin
			}

			for (auto iter = tls.ReadyFibers.begin(); iter != tls.ReadyFibers.end(); ++iter) 
			{
				if (!iter->second->load(std::memory_order_relaxed)) {
					continue;
				}

				// check if we can safely continue the waiting fiber, if so, deleted the readyfiber atomic flag.
				waitingFiberIndex = iter->first;
				delete iter->second;
				tls.ReadyFibers.erase(iter);
				break;
			}

			// Unlock
			tls.ReadyFibersLock.clear(std::memory_order_release);

			// If we found a waiting task which we can continue, continue that task.
			if (waitingFiberIndex != UINT32_MAX) 
			{
				// Found a waiting task that is ready to continue

				tls.OldFiberIndex = tls.CurrentFiberIndex;
				tls.CurrentFiberIndex = waitingFiberIndex;
				tls.OldFiberDestination = FiberDestination::ToPool;

				// Switch to the waiting fiber.
				scheduler->_mFibers[tls.OldFiberIndex].SwitchToFiber(&scheduler->_mFibers[tls.CurrentFiberIndex]);

				// We're done, clean up the old fiber.
				scheduler->CleanUpFiber();
			}
			else 
			{
				// Get a new task from the queue, and execute it
				FTaskBase* nextTask;
				bool gotTask = scheduler->GetNextTask(&nextTask);
				
				// If we got a task, execute it.
				if (gotTask)
				{
					nextTask->Execute(); // execute the task.
					if (nextTask->GetCounter() != nullptr) 
					{
						nextTask->GetCounter()->FetchSub(1);
					}
				}
				else
				{
					std::this_thread::yield();
				}
			}
		}


		// Switch to the thread fiber since we finished execution.
		FThreadLocalStorage &tls = scheduler->_mTLS[scheduler->GetCurrentThreadIndex()];
		scheduler->_mFibers[tls.CurrentFiberIndex].SwitchToFiber(&tls.ThreadFiber);
	}


	uint32_t RTaskScheduler::GetFreeFiberIndex() 
	{
		for (uint32_t j = 0; ; ++j)
		{
			for (uint32_t i = 0; i < _mNumFibers; ++i) {
				// Do a relaxed check if the fiber is free.
				if (!_mFreeFibers[i].load(std::memory_order_relaxed)) {
					continue;
				}

				// If the fiber is available, acquire the free fiber value.
				if (!_mFreeFibers[i].load(std::memory_order_acquire)) {
					continue;
				}

				// Do an atomic compare exchange, we expect the fiber to be free, if the fiber is free, exchange it with false and return this thread index.
				bool expected = true;
				if (std::atomic_compare_exchange_weak_explicit(&_mFreeFibers[i], &expected, false, std::memory_order_release, std::memory_order_relaxed)) {
					return i;
				}
			}

			if (j > 20)
			{
				RLOG_WARNING("Could not get a free fiber therefore there might be a deadlock.");
			}
		}
		
		// should never reach this code.
		return UINT32_MAX;
	}

	bool RTaskScheduler::GetNextTask(FTaskBase** nextTask)
	{
		uint32_t currentThreadIndex = GetCurrentThreadIndex();
		FThreadLocalStorage &tls = _mTLS[currentThreadIndex];

		// Try to pop from our own queue
		if (tls.TaskQueue.Pop(nextTask)) {
			return true;
		}

		// Try to steal a task from another thread, start at the last thread we were able to steal from since it has a higher chance it has work.
		const uint32_t threadIndex = tls.LastSuccessfulSteal;
		for (uint32_t i = 0; i < _mNumThreads; ++i) 
		{
			const uint32_t threadIndexToStealFrom = (threadIndex + i) % _mNumThreads;
			if (threadIndexToStealFrom == currentThreadIndex) 
			{
				continue;
			}

			FThreadLocalStorage &otherTLS = _mTLS[threadIndexToStealFrom];

			if (otherTLS.TaskQueue.Steal(nextTask)) 
			{
				tls.LastSuccessfulSteal = threadIndexToStealFrom;
				return true;
			}
		}

		return false;
	}

	void RTaskScheduler::CleanUpFiber()
	{
		FThreadLocalStorage &tls = _mTLS[GetCurrentThreadIndex()];

		switch (tls.OldFiberDestination) 
		{
			case FiberDestination::ToPool:
			{
				// The old fiber is free for usage again.
				_mFreeFibers[tls.OldFiberIndex].store(true, std::memory_order_release);

				tls.OldFiberDestination = FiberDestination::Unknown;
				tls.OldFiberIndex = UINT32_MAX;
			}
			break;
			case FiberDestination::ToWaiting:
			{
				// The old fiber is ready to be consumed since we switched out of it at this point.
				tls.pOldFiberStoredFlag->store(true, std::memory_order_relaxed);
				tls.OldFiberDestination = FiberDestination::Unknown;
				tls.OldFiberIndex = UINT32_MAX;
			}
			break;
			case FiberDestination::Unknown:
			default:
			break;
		}
	}

	void RTaskScheduler::AddReadyFiber(uint32_t fiberIndex, std::atomic<bool> *fiberStoredFlag)
	{
		FThreadLocalStorage *tls = &_mTLS[GetCurrentThreadIndex()];
	
		// Lock
		while (tls->ReadyFibersLock.test_and_set(std::memory_order_acquire)) {
			// Spin
		}

		// Add fiber to thread's ready fiber list.
		tls->ReadyFibers.emplace_back(fiberIndex, fiberStoredFlag);

		// Unlock
		tls->ReadyFibersLock.clear(std::memory_order_release);
	}
}