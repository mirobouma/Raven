#include "Core/Threading/TaskScheduler.h"

namespace Raven 
{
	void RTaskScheduler::Initialize(uint32_t numWorkers)
	{
		_mThreadPool = std::unique_ptr<RThreadPool>(new RThreadPool(numWorkers, [this](RThread* thread) { WorkerStart(thread); }));
	}

	void RTaskScheduler::Destroy()
	{
		_mThreadPool->Destroy();
	}

	void RTaskScheduler::WorkerStart(RThread* thread)
	{
		// entry point for worker threads.
	}
}