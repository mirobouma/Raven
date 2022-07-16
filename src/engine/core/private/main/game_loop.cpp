#include "core/main/game_loop.h"
#include "os/window_system.h"
#include "util/logging_macros.h"

namespace Raven
{
    Result GameLoop::initialize(bool isStandalone)
    {
        _isStandalone = isStandalone;

        if (isStandalone)
        {
            _mainWindow = WindowSystem::get().create_window("Raven", Vector2Int(1024, 800), WindowCreationFlags::WindowCreationFlags_Shown);
            RASSERT(_mainWindow != nullptr);
        }
        else
        {
            _mainWindow = WindowSystem::get().get_main_window();
            RASSERT(_mainWindow != nullptr);
        }

        return Result::SUCCESS;
    }

    void GameLoop::update(float deltaTime)
    {

    }

    Result GameLoop::cleanup()
    {
        if (_isStandalone)
        {
            WindowSystem::get().destroy_window(_mainWindow);
        }

        return Result::SUCCESS;
    }
}