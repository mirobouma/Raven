#include "Core/Debug/LogManager.h"

namespace Raven 
{
	// Global variable.
	RLogManager* GLogManager = nullptr;

	RLogManager::RLogManager(ELogSeverity minSeverity)
	{
		mLogger = std::unique_ptr<IPlatformLogger>(new RPlatformLogger(minSeverity));
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

		delete GLogManager;
		GLogManager = nullptr;
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
		GLogManager->mLogger->Log(severity, message, args);
		va_end(args);

	}
}