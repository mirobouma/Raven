#include "engine/windows/renderable_window.h"
#include "engine/globals.h"
#include "engine/engine.h"
#include "renderer_interface/renderer_interface.h"

namespace Raven 
{
	RRenderableWindow::RRenderableWindow(const char* title, FVector2 size, FVector2 position, EPlatformWindowFlags flags, IWindowMessageHandler* handler)
	: RPlatformWindow(title, size, position, flags, handler)
	{
		_SwapChain = nullptr;
	}

	void RRenderableWindow::OnCreate()
	{
		if (GEngine != nullptr)
		{
			GEngine->GetRenderer()->createSwapChain(this, &_SwapChain);
		}	

		// Call super.
		RPlatformWindow::OnCreate();
	}

	void RRenderableWindow::OnResize(int width, int height)
	{
		// Call super.
		RPlatformWindow::OnResize(width, height);

		if (GEngine->GetRenderer()->isInitialized())
		{

			GEngine->GetRenderer()->waitForDevice();

			GEngine->GetRenderer()->destroySwapChain(_SwapChain);
			GEngine->GetRenderer()->createSwapChain(this, &_SwapChain);

		}
	}

	void RRenderableWindow::OnMove(int x, int y)
	{
		// Call super.
		RPlatformWindow::OnMove(x, y);
	}

	void RRenderableWindow::OnFocus()
	{
		// Call super.
		RPlatformWindow::OnFocus();
	}

	void RRenderableWindow::OnFocusLost()
	{
		// Call super.
		RPlatformWindow::OnFocusLost();
	}

	void RRenderableWindow::OnClose() 
	{
		if (GEngine != nullptr)
		{
			// Destroy swapchain.
			GEngine->GetRenderer()->destroySwapChain(_SwapChain);
		}

		// Call super.
		RPlatformWindow::OnClose();
	}
}