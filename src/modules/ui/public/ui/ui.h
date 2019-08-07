#pragma once
#include "imgui/imgui.h"
#include "renderer_interface/renderer_interface.h"
#include "renderer_interface/swapchain.h"
#include "platform/interfaces/window.h"
#include "rendering/render_graph/render_graph.h"

#define MAX_UI_FRAMES 3

namespace Raven 
{
	class RUIRenderer 
	{
	public:
		RUIRenderer() : mInitialized(0), mCurrentFrameId(0), mBlendState(), mDepthStencilState(), mRasterizerState() {}

		/*! Initialize this ui renderer. */
		void initialize(IRenderInterface* pRenderer, IPlatformWindow* pWindow, ISwapChain* pSwapChain);

		/*! Starts a new frame. */
		void newFrame(ISwapChain* pSwapChain);

		/*! Renders the recorded ui. */
		FVirtualResourceRef* render(RRenderGraphBuilder* builder, FVirtualResourceRef* target);

		/*! Draw imgui profiler. */
		void drawProfiler();

		/*! Destroy resources used in this UI renderer.*/
		void destroy();

	private:
		IPlatformWindow* mWindow = nullptr;
		IRenderInterface* mRenderer = nullptr;

		IShader* mShader = nullptr;
		IRootSignature* mRootSignature = nullptr;
		IPipeline* mPipeline = nullptr;
		ISampler* mFontSampler = nullptr;
		IImage* mFontTexture = nullptr;

		IBuffer* mIndexBuffers[MAX_UI_FRAMES];
		IBuffer* mVertexBuffers[MAX_UI_FRAMES];

		FBlendState mBlendState;
		FDepthStencilState mDepthStencilState;
		FRasterizerState mRasterizerState;

		bool mInitialized;
		uint32_t mCurrentFrameId;
	};
}