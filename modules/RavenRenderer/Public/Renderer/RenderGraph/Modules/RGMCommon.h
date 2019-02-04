#pragma once
#include "Renderer/RenderGraph/IRenderGraphModule.h"

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
		virtual void Init(IRenderInterface* pRenderer);

		/*! Destroy function for any global state. */
		virtual void Destroy(IRenderInterface* pRenderer);

		/*! Load resources that need to change when input is different. */
		virtual void Load(IRenderInterface* pRenderer, ISwapChain* pSwapChain);

		/*! Unload resources. */
		virtual void Unload(IRenderInterface* pRenderer);

		/*! Acquire next swapchain image and import the target into the render graph system. */
		FRGMCommonBackbufferTargets RGAcquireBackbuffer(RRenderGraphBuilder* pBuilder, IRenderInterface* renderer, ISwapChain* swapChain);

		/*! Import a target into the system. */
		FVirtualResourceRef* RGImportTarget(RRenderGraphBuilder* pBuilder, std::string name, IRenderTarget* target);

		/*! Create a render graph target. */
		FVirtualResourceRef* RGCreateTarget(RRenderGraphBuilder* pBuilder, std::string name, FVirtualResourceDesc& desc);

		/*! Submit all the commands of the render graph and present the backbuffer. */
		void RGSubmitAndPresent(RRenderGraphBuilder* pBuilder, ISwapChain* pSwapchain, FVirtualResourceRef* backbuffer);

	};
}