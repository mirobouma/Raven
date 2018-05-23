#pragma once
#include "Profiler.h"
#include <map>
#include <atomic>
#include "Core/Threading/TaskScheduler.h"

namespace Raven
{
	class RProfiler;
	extern RProfiler* GProfiler;

	struct FProfilerData
	{
		std::string name;
		std::chrono::steady_clock::time_point begin;
		std::chrono::steady_clock::time_point end;
	};

	struct FProfilerScopeData
	{
		FProfilerScopeData() {}
		FProfilerScopeData(std::string name)
		{
			_startTime = std::chrono::high_resolution_clock::now();
			_name = name;
		}

		std::chrono::steady_clock::time_point _startTime;
		std::string _name;
	};

	struct FThreadProfiler
	{
		FThreadProfiler() : threadId(UINT32_MAX), inScope(false) {}
		FThreadProfiler(uint32_t id) : threadId(id) {}

		std::chrono::steady_clock::time_point _startTime;
		std::chrono::steady_clock::time_point _endTime;

		uint32_t threadId;
		std::vector<FProfilerData> data;
		FProfilerScopeData currentScope;
		bool inScope;
	};


	class RProfiler
	{
	private:
		RProfiler();
		~RProfiler();
	
	public:
		static void Initialize();
		static void Destroy();
		
		static void EnterScope(std::string task);
		static void ExitScope();
		static void ProfileNextFrame();
		static void StopProfiling();
		static void StartProfiling();
		static void SwitchContext();
		static FThreadProfiler* GetThreadProfiler();
		
	protected:
		std::map<uint32_t, FThreadProfiler> _mThreadProfilers;
		std::mutex							_mProfilerMutex;
		bool								_mProfiling;
		bool								_mProfileNext;
		std::chrono::steady_clock::time_point _startTime;

	public:
		std::map<uint32_t, FThreadProfiler> mSavedThreadProfilers;
	};
}


#define ENABLE_PROFILING
#ifdef ENABLE_PROFILING
#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#define PROFI_CONCAT(x, y) x ## y
#define PROFI_CONCAT2(x, y) PROFI_CONCAT(x, y)

#define PROFILER_BEGIN if(Raven::GProfiler != nullptr) { Raven::GProfiler->StartProfiling(); }
#define PROFILER_END if(Raven::GProfiler != nullptr) { Raven::GProfiler->StopProfiling(); }
#define PROFILER_START_FUNC if(Raven::GProfiler != nullptr) { Raven::GProfiler->EnterScope(__PRETTY_FUNCTION__); }
#define PROFILER_START_SCOPE(NAME) if(Raven::GProfiler != nullptr) { Raven::GProfiler->EnterScope(NAME); }
#define PROFILER_END_SCOPE  if(Raven::GProfiler != nullptr) { Raven::GProfiler->ExitScope(); }
#else
#define PROFILER_BEGIN
#define PROFILER_END
#define PROFILER_START_FUNC
#define PROFILER_START_SCOPE(NAME)
#define PROFILER_END_SCOPE
#endif