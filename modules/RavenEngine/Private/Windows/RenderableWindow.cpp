#include "Engine/Windows/RenderableWindow.h"
#include "Engine/Globals.h"
#include "Engine/Engine.h"

namespace Raven 
{
	RRenderableWindow::RRenderableWindow(const char* title, Math::FVector2 size, Math::FVector2 position, EPlatformWindowFlags flags, IWindowMessageHandler* handler)
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