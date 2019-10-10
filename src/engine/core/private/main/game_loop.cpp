#include "core/main/game_loop.h"
#include "core/os/window_system.h"
#include "core/logging_macros.h"

Error GameLoop::initialize(bool isStandalone)
{
    _isStandalone = isStandalone;

    if (isStandalone)
    {
        _mainWindow = WindowSystem::get().create_window("Raven", Vector2Int(1024, 800), WindowCreationFlags_Shown);
        RASSERT(_mainWindow != nullptr);  
    }
    else
    {
        _mainWindow = WindowSystem::get().get_main_window();
        RASSERT(_mainWindow != nullptr);
    }

    return Error::SUCCESS;
}

void GameLoop::update(float deltaTime)
{
    
}

Error GameLoop::cleanup()
{
    if (_isStandalone)
    {
        WindowSystem::get().destroy_window(_mainWindow);
    }
    
    return Error::SUCCESS;
}