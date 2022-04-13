#include "editor/editor_loop.h"
#include "core/os/window_system.h"
#include "core/logging_macros.h"

namespace Raven
{
	Error EditorLoop::initialize(bool _)
	{
		_isStandalone = true;

		_mainWindow = WindowSystem::get().create_window("Raven Editor", Vector2Int(1024, 800), 
			WindowCreationFlags::WindowCreationFlags_Shown);

		RASSERT(_mainWindow != nullptr);
		return Error::SUCCESS;
	}

	void EditorLoop::update(float deltaTime)
	{

	}

	Error EditorLoop::cleanup()
	{
		if (_isStandalone)
		{
			WindowSystem::get().destroy_window(_mainWindow);
		}

		return Error::SUCCESS;
	}
}