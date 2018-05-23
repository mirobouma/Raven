#include "Core/Debug/Profiler.h"
#include "Core/Threading/TaskScheduler.h"

namespace Raven
{
	RProfiler* GProfiler = nullptr;

	RProfiler::RProfiler() : _mProfilerMutex(), _mProfiling(false), _mProfileNext(false)
	{

	}

	RProfiler::~RProfiler()
	{

	}

	void RProfiler::Initialize()
	{
		RASSERT(GProfiler == nullptr);

		GProfiler = new RProfiler();
	}

	void RProfiler::Destroy()
	{
		RASSERT(GProfiler != nullptr);
		delete GProfiler;
		GProfiler = nullptr;
	}

	void RProfiler::EnterScope(std::string task)
	{
		if (GProfiler == nullptr)
		{
			RLOG_WARNING("Tried to enter scope but the profiler has not been initialized.");
			return;
		}

		if (GTaskScheduler == nullptr)
		{
			RLOG_WARNING("Tried to profile function but the task scheduler is not initialized yet, therefore the current context is not saved.");
			return;
		}

		if (!GProfiler->_mProfiling)
			return;

		uint32_t threadId = GTaskScheduler->GetCurrentThreadIndex();
		if (threadId == UINT32_MAX)
		{
			RLOG_WARNING("Tried to profile function but the thread is not registered in the task scheduler.");
			return;
		}

		auto it = GProfiler->_mThreadProfilers.find(threadId);

		if (it != GProfiler->_mThreadProfilers.end())
		{
			FThreadProfiler* profiler = &it->second;
			if (profiler->inScope)
			{
				ExitScope();
			}
			profiler->inScope = true;
			profiler->currentScope = FProfilerScopeData(task);
		}
		else
		{
			// Lock
			GProfiler->_mProfilerMutex.lock();
		
			GProfiler->_mThreadProfilers[threadId] = FThreadProfiler(threadId);
			GProfiler->_mThreadProfilers[threadId].currentScope = FProfilerScopeData(task);
			GProfiler->_mThreadProfilers[threadId]._startTime = GProfiler->_startTime;
			GProfiler->_mThreadProfilers[threadId].inScope = true;
			GProfiler->_mThreadProfilers[threadId].threadId = threadId;

			// Unlock
			GProfiler->_mProfilerMutex.unlock();
		}

	}

	void RProfiler::ExitScope()
	{
		if (GProfiler == nullptr)
		{
			RLOG_WARNING("Tried to enter scope but the profiler has not been initialized.");
			return;
		}

		if (!GProfiler->_mProfiling)
			return;

		if (GTaskScheduler == nullptr)
		{
			RLOG_WARNING("Tried to profile function but the task scheduler is not initialized yet, therefore the current context is not saved.");
			return;
		}

		uint32_t threadId = GTaskScheduler->GetCurrentThreadIndex();
		if (threadId == UINT32_MAX)
		{
			RLOG_WARNING("Tried to profile function but the thread is not registered in the task scheduler.");
		}

		auto it = GProfiler->_mThreadProfilers.find(threadId);

		if (it != GProfiler->_mThreadProfilers.end())
		{
			FThreadProfiler* profiler = &it->second;

			if (profiler->inScope)
			{
				FProfilerData data;
				auto currentTime = std::chrono::high_resolution_clock::now();
				data.end = currentTime;
				data.begin = profiler->currentScope._startTime;
				data.name = profiler->currentScope._name;
				profiler->inScope = false;

				profiler->data.push_back(data);
			}
		}
		else
		{
			RLOG_WARNING("Tried to exit function scope but the thread has not initialized its profiler context yet.");
		}
	}

	void RProfiler::StopProfiling()
	{
		if (GProfiler == nullptr)
		{
			RLOG_WARNING("Tried to stop profiling but the profiler has not been initialized.");
			return;
		}

		if (!GProfiler->_mProfiling)
			return;

		GProfiler->_mProfilerMutex.lock();

		if (GProfiler->_mProfiling)
		{
			auto currentTime = std::chrono::high_resolution_clock::now();

			for (uint32_t i = 0; i < GProfiler->_mThreadProfilers.size(); ++i)
			{
				GProfiler->_mThreadProfilers[i]._endTime = currentTime;
			}

			GProfiler->_mProfiling = false;
			GProfiler->mSavedThreadProfilers = GProfiler->_mThreadProfilers;
		}

		GProfiler->_mProfilerMutex.unlock();
	}

	void RProfiler::ProfileNextFrame()
	{
		if (GProfiler == nullptr)
		{
			RLOG_WARNING("Tried profile next frame but the profiler has not been initialized.");
			return;
		}

		GProfiler->_mProfileNext = true;
	}

	FThreadProfiler* RProfiler::GetThreadProfiler()
	{
		if (GProfiler == nullptr)
		{
			return nullptr;
		}

		uint32_t threadId = GTaskScheduler->GetCurrentThreadIndex();
		if (threadId == UINT32_MAX)
		{
			return nullptr;
		}

		auto profiler = GProfiler->_mThreadProfilers.find(threadId);

		if (profiler != GProfiler->_mThreadProfilers.end())
		{
			return &profiler->second;
		}

		return nullptr;
	}

	void RProfiler::StartProfiling()
	{
		if (GProfiler == nullptr)
		{
			RLOG_WARNING("Tried to start profiling but the profiler has not been initialized.");
			return;
		}

		if (GProfiler->_mProfiling)
		{
			RLOG_WARNING("Already started profiling, resetting profiler data.");
		}

		if (GProfiler->_mProfileNext)
		{
			GProfiler->_mProfiling = true;
			GProfiler->_mProfileNext = false;
		}


		if (GProfiler->_mProfiling)
		{
			GProfiler->_mProfilerMutex.lock();

			auto currentTime = std::chrono::high_resolution_clock::now();

			for (uint32_t i = 0; i < GProfiler->_mThreadProfilers.size(); ++i)
			{
				GProfiler->_mThreadProfilers[i].data.clear();
				GProfiler->_mThreadProfilers[i]._startTime = currentTime;
			}
			GProfiler->_startTime = currentTime;
			GProfiler->_mProfilerMutex.unlock();
		}
	}
}