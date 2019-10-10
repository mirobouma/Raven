#pragma once
#include "core/main/main_loop.h"

class Window;
class GameLoop : public MainLoop
{
public:
    virtual Error initialize(bool isStandalone);
    virtual void update(float deltaTime);
    virtual Error cleanup();

private:
    Window* _mainWindow;
};