#include "GameWindow.h"
#include "Engine/Globals.h"
#include "Engine/Engine.h"

namespace Raven
{
    RGameWindow::RGameWindow(const char* title, Math::FVector2 size, Math::FVector2 position, EPlatformWindowFlags flags, IWindowMessageHandler* handler)
            : RRenderableWindow(title, size, position, flags, handler)
    {

    }

    void RGameWindow::OnCreate()
    {
        // Call super.
        RPlatformWindow::OnCreate();
    }

    void RGameWindow::OnResize(int width, int height)
    {
        // Call super.
        RPlatformWindow::OnResize(width, height);
    }

    void RGameWindow::OnMove(int x, int y)
    {
        // Call super.
        RPlatformWindow::OnMove(x, y);
    }

    void RGameWindow::OnFocus()
    {
        // Call super.
        RPlatformWindow::OnFocus();
    }

    void RGameWindow::OnFocusLost()
    {
        // Call super.
        RPlatformWindow::OnFocusLost();
    }

    void RGameWindow::OnClose()
    {
        // Call super.
        RPlatformWindow::OnClose();
    }

    void RGameWindow::Render()
    {

    }
}