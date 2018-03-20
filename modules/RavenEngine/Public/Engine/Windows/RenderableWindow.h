#pragma once
#include "HAL/PlatformWindow.h"
#include "Rendering/Interfaces/ISwapChain.h"

namespace Raven {
	/*!
	\class RRenderableWindow
	\brief Represents a renderable window, will initialize a swapchain for the surface on window creation, and destroy it accordingly.
	*/
	class RRenderableWindow : public RPlatformWindow {
	public:
		RRenderableWindow(const char* title, Math::FVector2 size, Math::FVector2 position, EPlatformWindowFlags flags, IWindowMessageHandler* handler);
		RRenderableWindow(const RRenderableWindow&) = delete; /*!< Delete copy constuctor since it doesn't make sense to copy a window. */
		RRenderableWindow& operator= (const RRenderableWindow&) = delete; /*!< Delete assignment constuctor since it doesn't make sense to overwrite a window. */

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

		/*! Swapchain reference for this window. */
		ISwapChain* _SwapChain;
	};
}