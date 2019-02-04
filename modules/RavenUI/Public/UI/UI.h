#pragma once
#include "imgui/imgui.h"
#include "RenderInterface/IRenderInterface.h"
#include "RenderInterface/ISwapChain.h"
#include "Platform/Interfaces/IPlatformWindow.h"
#include "Renderer/RenderGraph/RenderGraph.h"

#define MAX_UI_FRAMES 3

namespace Raven 
{
	class RUIRenderer 
	{
	public:
		RUIRenderer() : mInitialized(0), mCurrentFrameId(0) {}

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
		IPlatformWindow* mWindow;
		IRenderInterface* mRenderer;

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