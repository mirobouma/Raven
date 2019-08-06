#include "rendering/render_graph/modules/rgm_common.h"

namespace Raven
{
	void RGMCommon::Init(IRenderInterface* pRenderer)
	{	 
	}	 
		 
	void RGMCommon::Destroy(IRenderInterface* pRenderer)
	{	 

	}	 
		 
	void RGMCommon::Load(IRenderInterface* pRenderer, ISwapChain* pSwapChain)
	{	 
	}	 
		 
	void RGMCommon::Unload(IRenderInterface* pRenderer)
	{
	}

	FRGMCommonBackbufferTargets RGMCommon::RGAcquireBackbuffer(RRenderGraphBuilder* pBuilder, IRenderInterface* renderer, ISwapChain* swapChain)
	{
		FRGMCommonBackbufferTargets output;

		// Add the pass and setup its inputs and outputs.
		auto pass = pBuilder->add_pass("get_swapchain_target", [&](RRenderPassBuilder& builder)
		{
			// Acquire swapchain image.
			renderer->swapChainAcquireNextImage(swapChain, swapChain->getImageAcquiredSemaphore(), nullptr);

			// import backbuffer resource.
			FImportResourceDesc backbufferImport;
			backbufferImport.mType = VIRTUAL_RESOURCE_TYPE_IMPORTED_RENDER_TARGET;
			backbufferImport.pRenderTarget = swapChain->getRenderTarget(swapChain->getCurrentImageIndex());

			// import backbuffer resource.
			output.pBackbuffer = builder.import_resource("backbuffer", backbufferImport);

			// Make sure we never cull this render pass.
			builder.setDoNotCull(true);
		});

		return output;
	}

	FVirtualResourceRef* RGMCommon::RGImportTarget(RRenderGraphBuilder* pBuilder, std::string name, IRenderTarget* target)
	{
		FVirtualResourceRef* output = nullptr;

		// Add the pass and setup its inputs and outputs.
		auto pass = pBuilder->add_pass("import_resource", [&](RRenderPassBuilder& builder)
		{
			// import resource.
			FImportResourceDesc import;
			import.mType = VIRTUAL_RESOURCE_TYPE_IMPORTED_RENDER_TARGET;
			import.pRenderTarget = target;

			// import resource.
			output = builder.import_resource(name.c_str(), import);
		});

		return output;
	}

	FVirtualResourceRef* RGMCommon::RGCreateTarget(RRenderGraphBuilder* pBuilder, std::string name, FVirtualResourceDesc& desc)
	{
		FVirtualResourceRef* output = nullptr;

		// Add the pass and setup its inputs and outputs.
		auto pass = pBuilder->add_pass("create_resource", [&](RRenderPassBuilder& builder)
		{
			// import resource.
			output = builder.create_resource(name.c_str(), desc);
		});

		return output;
	}

	void RGMCommon::RGSubmitAndPresent(RRenderGraphBuilder* pBuilder, ISwapChain* pSwapchain, FVirtualResourceRef* backbuffer)
	{
		// Add the pass and setup its inputs and outputs.
		auto pass = pBuilder->add_pass("submit_data", [&](RRenderPassBuilder& builder)
		{
			// Make sure we never cull this render pass.
			builder.setDoNotCull(true);

			// We need to present the backbuffer.
			builder.read_resource("backbuffer", backbuffer, RENDER_GRAPH_RESOURCE_ACCESS_GPU, RENDER_GRAPH_RESOURCE_READ_STATE_PRESENT);
		});

		// Set execute function.
		pass->setExecuteFunc([=](IRenderInterface* renderer, RRenderGraph& graph, RCompiledPass& pass, ICommandList* cmd)
		{
			// End command recording for this command list.
			renderer->cmdEnd(cmd);

			ISwapChain* swapchains = { pSwapchain };
			ISemaphore* waitSemaphores = { pSwapchain->getImageAcquiredSemaphore() };

			renderer->queueSubmit(graph.mManager->GetGraphicsQueue(), 1, &cmd, graph.mRenderCompleteFence, 1, &waitSemaphores, 1, &graph.mRenderCompleteSemaphore);
			renderer->queuePresent(graph.mManager->GetGraphicsQueue(), 1, &swapchains, 1, &graph.mRenderCompleteSemaphore);
		});
	}
}
