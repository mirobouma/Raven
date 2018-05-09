#pragma once
#include "Engine/Windows/RenderableWindow.h"
#include "Rendering/Interfaces/ISwapChain.h"
#include "Rendering/Interfaces/IRenderer.h"

namespace Raven
{
    /*!
    \class RGameWindow
    \brief Represents a game window, will initialize a swapchain for the surface on window creation and render the scene, and destroy it accordingly.
    */
    class RGameWindow : public RRenderableWindow {
    public:
        RGameWindow(const char* title, Math::FVector2 size, Math::FVector2 position, EPlatformWindowFlags flags, IWindowMessageHandler* handler);
        RGameWindow(const RGameWindow&) = delete; /*!< Delete copy constuctor since it doesn't make sense to copy a window. */
        RGameWindow& operator= (const RGameWindow&) = delete; /*!< Delete assignment constuctor since it doesn't make sense to overwrite a window. */

        /*! Render function */
        void Render();

		void LoadTeapot();

    protected:
        /*! Callback function when the window is created. */
        virtual void OnCreate() override;

        /*! Callback function when the window is resized. */
        virtual void OnResize(int width, int height) override;

        /*! Callback function when the window is moved. */
        virtual void OnMove(int x, int y) override;

        /*! Callback function when the window is focussed. */
        virtual void OnFocus() override;

        /*! Callback function when the window focus is lost. */
        virtual void OnFocusLost() override;

        /*! Callback function when the window posts a quit message. */
        virtual void OnClose() override;

		IRenderer* renderer;
    };
}