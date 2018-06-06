#pragma once
#include "Rendering/RenderGraph/IRenderGraphModule.h"

namespace Raven
{
	/*!\struct FRGMCommonTargets
	\brief Resources returned by the setup function of the render graph module. 
	*/
	struct FRGMCommonBackbufferTargets
	{
		FVirtualResourceRef* pBackbuffer;
	};
	
	/*!\class RGMCommon
	\brief Render graph module for common functionality.
	*/
	class RGMCommon : public IRenderGraphModule
	{
	public:
		/*! Initialization function for any global state (shaders, persistent targets) */
		virtual void Init(IRenderer* pRenderer);

		/*! Destroy function for any global state. */
		virtual void Destroy(IRenderer* pRenderer);

		/*! Load resources that need to change when input is different. */
		virtual void Load(IRenderer* pRenderer, ISwapChain* pSwapChain);

		/*! Unload resources. */
		virtual void Unload(IRenderer* pRenderer);

		/*! Acquire next swapchain image and import the target into the render graph system. */
		FRGMCommonBackbufferTargets RGAcquireBackbuffer(RRenderGraphBuilder* pBuilder, IRenderer* renderer, ISwapChain* swapChain);

		/*! Import a target into the system. */
		FVirtualResourceRef* RGImportTarget(RRenderGraphBuilder* pBuilder, IRenderer* renderer, std::string name, IRenderTarget* target);

		/*! Submit all the commands of the render graph and present the backbuffer. */
		void RGSubmitAndPresent(RRenderGraphBuilder* pBuilder, ISwapChain* pSwapchain, FVirtualResourceRef* backbuffer);

	};
}