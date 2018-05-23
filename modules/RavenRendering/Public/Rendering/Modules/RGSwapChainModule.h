#pragma once
#include "Rendering/RenderGraph.h"

namespace Raven
{
	namespace RGSwapChainModule
	{
		struct RGSwapChainModuleData
		{
			FRenderGraphResource output_backbuffer;
		};

		RGSwapChainModuleData rendergraph_add_acquire_target(RRenderGraphBuilder& builder, IRenderer* renderer, ISwapChain* swapChain, ISemaphore* signalSemaphore, IFence* signalFence)
		{
			return builder.add_lambda_render_pass<RGSwapChainModuleData>(
				"get_swapchain_target",
				[&](RRenderPassBuilder& builder, RGSwapChainModuleData& data)
				{
					// Acquire swapchain image.
					renderer->swapChainAcquireNextImage(swapChain, signalSemaphore, signalFence);

					// import backbuffer resource.
					FImportResourceDesc backbufferImport;
					backbufferImport.mType = RENDER_GRAPH_RESOURCE_TYPE_IMPORTED_RENDER_TARGET;
					backbufferImport.pRenderTarget = swapChain->getRenderTarget(swapChain->getCurrentImageIndex());

					data.output_backbuffer = builder.import_resource("backbuffer", backbufferImport);

					// Make sure we never cull this render pass.
					builder.mDoNotCull = true;
				},
				[=](RRenderGraph& graph, const RGSwapChainModuleData& data)
				{
					// Get target.
					//IRenderTarget* target = graph.getRenderTargetResource(&data.output_backbuffer);
				});
		}
	};
}