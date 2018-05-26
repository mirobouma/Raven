#pragma once
#include "Rendering/RenderGraph.h"

namespace Raven
{
	namespace RGSwapChainModule
	{
		struct RGSwapChainModuleOutputs
		{
			FVirtualResourceRef* backbuffer;
		};

		RGSwapChainModuleOutputs rendergraph_add_acquire_target(RRenderGraphBuilder& builder, IRenderer* renderer, ISwapChain* swapChain, ISemaphore* signalSemaphore, IFence* signalFence)
		{
			RGSwapChainModuleOutputs output;

			// Add the pass and setup its inputs and outputs.
			auto pass = builder.add_pass("get_swapchain_target", [&](RRenderPassBuilder& builder)
			{
				// Acquire swapchain image.
				renderer->swapChainAcquireNextImage(swapChain, signalSemaphore, signalFence);

				// import backbuffer resource.
				FImportResourceDesc backbufferImport;
				backbufferImport.mType = VIRTUAL_RESOURCE_TYPE_IMPORTED_RENDER_TARGET;
				backbufferImport.pRenderTarget = swapChain->getRenderTarget(swapChain->getCurrentImageIndex());

				// import backbuffer resource.
				output.backbuffer = builder.import_resource("backbuffer", backbufferImport);

				// Make sure we never cull this render pass.
				builder.setDoNotCull(true);
			});

			pass->setExecuteFunc([=](RRenderGraph& graph, RCompiledPass& pass)
			{
				//IRenderTarget* target = pass.getRenderTargetResource("backbuffer");
			});

			// Return output resource references from the setup function.
			return output;

		}
	};
}