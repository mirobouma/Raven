#include "Core/Input/InputManager.h"
#include "Core/Debug/LogManager.h"
#include <algorithm>

namespace Raven
{
	// Global variable.
	RInputManager* GInputManager = nullptr;

	RInputManager::RInputManager()
	{
		_mInputManager.SetDisplaySize(1024, 1024);

		_mMouseId = _mInputManager.CreateDevice<gainput::InputDeviceMouse>();
		_mKeyboardId = _mInputManager.CreateDevice<gainput::InputDeviceKeyboard>();
		_mPadId = _mInputManager.CreateDevice<gainput::InputDevicePad>();
	}

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

	void RInputManager::SetDisplaySize(uint32_t width, uint32_t height)
	{
		_mInputManager.SetDisplaySize(width, height);
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