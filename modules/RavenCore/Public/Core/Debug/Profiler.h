#pragma once
#include "Profiler.h"
#include <map>
#include <atomic>
#include "Core/Threading/TaskScheduler.h"

#define ENABLE_PROFILING
#ifdef ENABLE_PROFILING
#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#define PROFI_CONCAT(x, y) x ## y
#define PROFI_CONCAT2(x, y) PROFI_CONCAT(x, y)

#define PROFILER_START_FUNC profi::LocalProfileScope PROFI_CONCAT2(lps_, __LINE__)(__PRETTY_FUNCTION__);
#define PROFILER_START_SCOPE(NAME) profi::LocalProfileScope PROFI_CONCAT2(lpss_, __LINE__)(NAME);

#else
#define PROFILER_START_FUNC
#define PROFILER_START_SCOPE(NAME)
#endif


namespace Raven
{
	struct FProfilerData
	{
		std::string name;
		uint64_t length;
	};

	struct FProfilerScope
	{
		FProfilerScope() {}
		FProfilerScope(std::string name)
		{
			_startTime = std::chrono::high_resolution_clock::now();
			_name = name;
		}

		std::chrono::steady_clock::time_point _startTime;
		std::string _name;
	};

	struct FThreadProfiler
	{
		FThreadProfiler() : threadId(UINT32_MAX) {}
		FThreadProfiler(uint32_t id) : threadId(id) {}

		std::chrono::steady_clock::time_point _startTime;
		std::chrono::steady_clock::time_point _endTime;

		uint32_t threadId;
		std::vector<FProfilerData> data;
		FProfilerScope currentScope;
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
		
	protected:
		std::map<uint32_t, FThreadProfiler> _mThreadProfilers;
		std::map<uint32_t, FThreadProfiler> _mSavedThreadProfilers;
		std::mutex							_mProfilerMutex;
		bool								_mProfiling;
		bool								_mProfileNext;
	};

	extern RProfiler* GProfiler;
}