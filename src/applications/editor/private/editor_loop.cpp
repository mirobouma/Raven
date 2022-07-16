#include "editor/editor_loop.h"
#include "os/window_system.h"
#include "util/logging_macros.h"

namespace Raven
{
	Result EditorLoop::initialize(bool _)
	{
		_isStandalone = true;
		_mainWindow = WindowSystem::get().create_window("Raven Editor", Vector2Int(1024, 800), 
			WindowCreationFlags::WindowCreationFlags_Shown);

		RASSERT(_mainWindow != nullptr);
		return Result::SUCCESS;
	}

	void EditorLoop::update(float deltaTime)
	{

	}

	Result EditorLoop::cleanup()
	{
		if (_isStandalone)
		{
			WindowSystem::get().destroy_window(_mainWindow);
		}

		return Result::SUCCESS;
	}
}