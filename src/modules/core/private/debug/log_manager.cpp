#include "core/debug/log_manager.h"
#include <algorithm>

namespace Raven 
{
	// Global variable.
	RLogManager* GLogManager = nullptr;

	RLogManager::RLogManager(ELogSeverity minSeverity)
	{
		_pLogger = new RPlatformLogger(minSeverity);
		mLoggers.push_back(_pLogger);
	}

	bool RLogManager::Initialize(ELogSeverity minSeverity)
	{
		if (GLogManager != nullptr)
		{
			// The log manager is already initialized.
			return false;
		}

		GLogManager = new RLogManager(minSeverity);

		RLOG_INFO("Initialized logging manager.");
		return true;
	}

	void RLogManager::Destroy()
	{
		if (GLogManager == nullptr)
		{
			return;
		}

		delete GLogManager->_pLogger;

		delete GLogManager;
		GLogManager = nullptr;
	}

	void RLogManager::RegisterLogger(ILogger* logger)
	{
		if (GLogManager == nullptr)
		{
			// The log manager does not exist.
			return;
		}

		GLogManager->mLoggers.push_back(logger);
	}

	void RLogManager::DeregisterLogger(ILogger* logger)
	{
		if (GLogManager == nullptr)
		{
			// The log manager does not exist.
			return;
		}

		GLogManager->mLoggers.erase(std::remove(GLogManager->mLoggers.begin(), GLogManager->mLoggers.end(), logger));
	}

	void RLogManager::Log(ELogSeverity severity, std::string message, ...)
	{
		if (GLogManager == nullptr)
		{
			// The log manager does not exist.
			return;
		}

		va_list args;
		va_start(args, message);

		for (uint32_t i = 0; i < GLogManager->mLoggers.size(); ++i)
		{
			GLogManager->mLoggers[i]->Log(severity, message, args);
		}
		va_end(args);

	}
}