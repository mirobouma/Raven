#include "core/input/input_manager.h"
#include "core/debug/log_manager.h"
#include <algorithm>

namespace Raven
{
	// Global variable.
	RInputManager* GInputManager = nullptr;
	RInputManager::RInputManager()
	{}

	bool RInputManager::Initialize()
	{
		if (GInputManager != nullptr)
		{
			// The log manager is already initialized.
			return false;
		}

		GInputManager = new RInputManager();

		RLOG_INFO("Initialized input manager.");
		return true;
	}

	RInputMapping* RInputManager::CreateMapping()
	{
		RInputMapping* map = new RInputMapping(this);
		_mMappings.push_back(std::unique_ptr<RInputMapping>(map));

		return map;
	}

	void RInputManager::Destroy()
	{
		if (GInputManager == nullptr)
		{
			return;
		}

		delete GInputManager;
		GInputManager = nullptr;
	}
}
