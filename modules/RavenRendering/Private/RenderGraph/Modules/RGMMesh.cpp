#include "Rendering/RenderGraph/Modules/RGMMesh.h"
#include <chrono>

namespace Raven
{
	void RGMMesh::Init(IRenderer* pRenderer)
	{
		FSamplerCreateDesc samplerDesc;
		samplerDesc.addressModeU = ESamplerAddressMode::SAMPLER_ADDRESS_MODE_REPEAT;
		samplerDesc.addressModeV = ESamplerAddressMode::SAMPLER_ADDRESS_MODE_REPEAT;
		samplerDesc.addressModeW = ESamplerAddressMode::SAMPLER_ADDRESS_MODE_REPEAT;
		samplerDesc.anisotropyEnable = true;
		samplerDesc.comparisonFunc = ESamplerComparisonFunc::SAMPLER_COMPARISON_NONE;
		samplerDesc.anisotropyEnable = true;
		samplerDesc.borderColor = EBorderColor::BORDER_COLOR_FLOAT_OPAQUE_BLACK;
		samplerDesc.maxAnisotropy = 16;
		samplerDesc.magFilter = ESamplerFilter::SAMPLER_FILTER_LINEAR;
		samplerDesc.minFilter = ESamplerFilter::SAMPLER_FILTER_LINEAR;
		samplerDesc.maxLod = 1000.0f;
		samplerDesc.minLod = 0.0f;
		samplerDesc.mipLodBias = 0.0f;
		samplerDesc.mipmapMode = EMipMode::MIP_MODE_LINEAR;

		pRenderer->createSampler(&mSampler, &samplerDesc);

		// Create shaders.
		FShaderCreateDesc shaderDesc = {};
		shaderDesc.shaders[0].path = "shaders/test.vert.spv";
		shaderDesc.shaders[0].stage = EShaderStage::SHADER_STAGE_VERTEX;
		shaderDesc.shaders[1].path = "shaders/test.frag.spv";
		shaderDesc.shaders[1].stage = EShaderStage::SHADER_STAGE_FRAGMENT;

		pRenderer->createShader(&mShader, &shaderDesc);

		// Create input assembler layout
		FInputAttributeDescription iaAttributes[5];
		iaAttributes[0].binding = 0;
		iaAttributes[0].format = TEXTURE_FORMAT_RGB32_FLOAT;
		iaAttributes[0].location = 0;
		iaAttributes[0].offset = 0;

		iaAttributes[1].binding = 0;
		iaAttributes[1].format = TEXTURE_FORMAT_RGB32_FLOAT;
		iaAttributes[1].location = 1;
		iaAttributes[1].offset = sizeof(glm::vec3);

		iaAttributes[2].binding = 0;
		iaAttributes[2].format = TEXTURE_FORMAT_RGB32_FLOAT;
		iaAttributes[2].location = 2;
		iaAttributes[2].offset = sizeof(glm::vec3) * 2;

		iaAttributes[3].binding = 0;
		iaAttributes[3].format = TEXTURE_FORMAT_RGB32_FLOAT;
		iaAttributes[3].location = 3;
		iaAttributes[3].offset = sizeof(glm::vec3) * 3;

		iaAttributes[4].binding = 0;
		iaAttributes[4].format = TEXTURE_FORMAT_RG32_FLOAT;
		iaAttributes[4].location = 4;
		iaAttributes[4].offset = sizeof(glm::vec3) * 4;

		// Create input assembler binding
		FInputBindingDescription iaBinding;
		iaBinding.binding = 0;
		iaBinding.inputRate = EVertexInputRate::VERTEX_INPUT_RATE_VERTEX;
		iaBinding.stride = sizeof(FMeshVertex);

		// Create input assembler layout
		FInputAssemblerLayout iaLayout;
		iaLayout.numInputBindingDescriptions = 1;
		iaLayout.pInputBindingDescriptions = &iaBinding;
		iaLayout.numInputAttributeDescriptions = 5;
		iaLayout.pInputAttributeDescriptions = &iaAttributes[0];

		// Create descriptor bindings.
		FDescriptorLayoutBinding bindings[4];
		bindings[0].set = 0;
		bindings[0].binding = 0;
		bindings[0].descriptorCount = 1;
		bindings[0].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		bindings[0].stageFlags = SHADER_STAGE_FLAG_VERTEX;

		bindings[1].set = 1;
		bindings[1].binding = 0;
		bindings[1].descriptorCount = 1;
		bindings[1].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		bindings[1].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		bindings[2].set = 1;
		bindings[2].binding = 1;
		bindings[2].descriptorCount = 1;
		bindings[2].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLER;
		bindings[2].ppImmutableSamplers = &mSampler;
		bindings[2].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		bindings[3].set = 2;
		bindings[3].binding = 0;
		bindings[3].descriptorCount = 1;
		bindings[3].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		bindings[3].ppImmutableSamplers = &mSampler;
		bindings[3].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		// Create root signature.
		FRootSignatureCreateDesc rsDesc;
		rsDesc.pShaderProgram = mShader;
		rsDesc.pInputAssemblerLayout = &iaLayout;
		rsDesc.numDescriptorBindings = 3;
		rsDesc.pDescriptorBindings = &bindings[0];

		pRenderer->createRootSignature(&mRootSignature, &rsDesc);
	}

	void RGMMesh::Destroy(IRenderer* pRenderer)
	{
		pRenderer->destroyShader(mShader);
		pRenderer->destroySampler(mSampler);
		pRenderer->destroyRootSignature(mRootSignature);
	}

	void RGMMesh::Load(IRenderer* pRenderer, ISwapChain* pSwapChain)
	{
		// Create graphics pipeline.
		FGraphicsPipelineCreateDesc pipelineDesc = {};
		pipelineDesc.pRootSignature = mRootSignature;
		pipelineDesc.pShader = mShader;
		pipelineDesc.pBlendState = nullptr;
		pipelineDesc.pDepthStencilState = nullptr;
		pipelineDesc.pRasterizerState = nullptr;
		pipelineDesc.depthStencilFormat = TEXTURE_FORMAT_D32_FLOAT;
		pipelineDesc.numRenderTargets = 1;
		pipelineDesc.renderTargetFormats[0] = TEXTURE_FORMAT_RGBA8;
;
		pipelineDesc.primitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE;
		pipelineDesc.sampleCount = pSwapChain->getRenderTarget(0)->GetDesc()->sampleCount;
		pipelineDesc.sampleQuality = pSwapChain->getRenderTarget(0)->GetDesc()->sampleQuality;

		pRenderer->createPipeline(&mPipeline, &pipelineDesc);
	}

	void RGMMesh::Unload(IRenderer* pRenderer)
	{
		pRenderer->destroyPipeline(mPipeline);
	}

	FRGMForwardTargets RGMMesh::RGDrawMeshesForward(RRenderGraphBuilder* pBuilder, FVirtualResourceRef* pColor, FVirtualResourceRef* pDepth, uint32_t numMeshes, RMesh** ppMeshes)
	{
		FRGMForwardTargets output;

		// Add the pass and setup its inputs and outputs.
		auto pass = pBuilder->add_pass("draw_meshes_forward", [&](RRenderPassBuilder& builder)
		{
			if (pColor != nullptr)
			{
				output.pColor = builder.write_resource("color", pColor, RENDER_GRAPH_RESOURCE_ACCESS_GPU, RENDER_GRAPH_RESOURCE_WRITE_STATE_COLOR_ATTACHMENT);
			}

			if (pDepth != nullptr)
			{
				output.pDepth = builder.write_resource("depth", pDepth, RENDER_GRAPH_RESOURCE_ACCESS_GPU, RENDER_GRAPH_RESOURCE_WRITE_STATE_DEPTH_WRITE);
			}
		});

		// Set execute function.
		pass->setExecuteFunc([=](IRenderer* renderer, RRenderGraph& graph, RCompiledPass& pass, ICommandList* cmd)
		{
			FLoadStoreOps loadStoreOps = {};
			loadStoreOps.colorLoadOps[0] = LOAD_OP_CLEAR;
			loadStoreOps.colorStoreOps[0] = STORE_OP_STORE;
			loadStoreOps.colorClearValues[0][0] = 0.529f;
			loadStoreOps.colorClearValues[0][1] = 0.808f;
			loadStoreOps.colorClearValues[0][2] = 0.980f;
			loadStoreOps.colorClearValues[0][3] = 1.0f;
			loadStoreOps.depthLoadOp = LOAD_OP_CLEAR;
			loadStoreOps.depthStoreOp = STORE_OP_STORE;
			loadStoreOps.depthClearValue = 1.0f;

			IRenderTarget* color = pass.getRenderTargetResource("color");
			IRenderTarget* depth = pass.getRenderTargetResource("depth");

			// Begin rendering pass.
			renderer->cmdBeginRenderPass(cmd, 1, &color, depth, &loadStoreOps);

			// Bind graphics pipeline.
			renderer->cmdBindPipeline(cmd, mPipeline);
			renderer->cmdSetViewport(cmd, FRect({ color->GetDesc()->width,  color->GetDesc()->height }, { 0, 0 }));
			renderer->cmdSetScissorRects(cmd, FRect({ color->GetDesc()->width,  color->GetDesc()->height }, { 0, 0 }));

			static auto startTime = std::chrono::high_resolution_clock::now();
			
			auto currentTime = std::chrono::high_resolution_clock::now();
			float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count() / 10.0f;

			// Create dynamic uniform allocation.
			FDynamicUniformAllocation cameraUniformAlloc;
			mUniforms.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			mUniforms.view = glm::lookAt(glm::vec3(3.0f, 3.0f, 3.0f), glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			mUniforms.proj = glm::perspective(glm::radians(45.0f), color->GetDesc()->width / (float)color->GetDesc()->height, 0.1f, 100.0f);
			mUniforms.proj[1][1] *= -1;

			renderer->cmdAllocateDynamicUniformBuffer(cmd, &cameraUniformAlloc, sizeof(FForwardMeshUniforms), &mUniforms);

			FDescriptorBinding bindings[1];
			bindings[0].set = 0;
			bindings[0].binding = 0;
			bindings[0].numDescriptors = 1;
			bindings[0].type = DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			bindings[0].ppBuffers = &cameraUniformAlloc.buffer;
			bindings[0].range = cameraUniformAlloc.range;
			bindings[0].offset = cameraUniformAlloc.offset;

			// Bind uniform buffer.
			renderer->cmdBindDescriptors(cmd, 1, &bindings[0]);

			for (uint32_t i = 0; i < numMeshes; ++i)
			{
				RMesh* mesh = ppMeshes[i];

				renderer->cmdBindIndexBuffer(cmd, mesh->mIndexBuffer);
				renderer->cmdBindVertexBuffer(cmd, 1, &mesh->mVertexBuffer);

				for (uint32_t i = 0; i < mesh->mSubMeshes.size(); ++i)
				{
					bindings[0].set = 1;
					bindings[0].binding = 0;
					bindings[0].numDescriptors = 1;
					bindings[0].type = DESCRIPTOR_TYPE_SAMPLED_IMAGE;
					bindings[0].ppTextures = &mesh->mSubMeshes[i].mMaterial->mTextures[TEXTURE_SLOT_ALBEDO];
					bindings[0].range = 0; // range 0 means full range.
					bindings[0].offset = 0;

					// Bind resources.
					renderer->cmdBindDescriptors(cmd, mesh->mSubMeshes[i].mMaterial->mTextures[TEXTURE_SLOT_ALBEDO] != nullptr ? 1 : 0, &bindings[0]);

					// Draw mesh.
					renderer->cmdDrawIndexed(cmd, mesh->mSubMeshes[i].mIndexCount, mesh->mSubMeshes[i].mIndexOffset, mesh->mSubMeshes[i].mVertexOffset);
				}
			}

			// Finish rendering pass.
			renderer->cmdEndRenderPass(cmd);
		});

		return output;
	}
}
