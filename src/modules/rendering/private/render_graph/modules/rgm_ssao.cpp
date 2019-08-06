#include "rendering/render_graph/modules/rgm_ssao.h"
#include <chrono>
#include "stb_image.h"
#include "core/debug/logmanager.h"
#include <random>

namespace Raven
{
	void RGMSSAO::Init(IRenderInterface* pRenderer)
	{
		FSamplerCreateDesc samplerDesc;
		samplerDesc.addressModeU = ESamplerAddressMode::SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerDesc.addressModeV = ESamplerAddressMode::SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerDesc.addressModeW = ESamplerAddressMode::SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerDesc.anisotropyEnable = false;
		samplerDesc.comparisonFunc = ESamplerComparisonFunc::SAMPLER_COMPARISON_NONE;
		samplerDesc.borderColor = EBorderColor::BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		samplerDesc.maxAnisotropy = 1;
		samplerDesc.magFilter = ESamplerFilter::SAMPLER_FILTER_LINEAR;
		samplerDesc.minFilter = ESamplerFilter::SAMPLER_FILTER_LINEAR;
		samplerDesc.maxLod = 0.0f;
		samplerDesc.minLod = 0.0f;
		samplerDesc.mipLodBias = 0.0f;
		samplerDesc.mipmapMode = EMipMode::MIP_MODE_NEAREST;

		pRenderer->createSampler(&mSampler, &samplerDesc);

		// Create shaders.
		FShaderCreateDesc shaderDesc = {};
		shaderDesc.shaders[0].path = "assets/shaders/ssao_depth.vert.spv";
		shaderDesc.shaders[0].stage = EShaderStage::SHADER_STAGE_VERTEX;
		shaderDesc.shaders[1].path = "assets/shaders/ssao_depth.frag.spv";
		shaderDesc.shaders[1].stage = EShaderStage::SHADER_STAGE_FRAGMENT;

		pRenderer->createShader(&mShader, &shaderDesc);

		// Create shaders.
		FShaderCreateDesc shaderDesc2 = {};
		shaderDesc2.shaders[0].path = "assets/shaders/ssao_deferred.vert.spv";
		shaderDesc2.shaders[0].stage = EShaderStage::SHADER_STAGE_VERTEX;
		shaderDesc2.shaders[1].path = "assets/shaders/ssao_deferred.frag.spv";
		shaderDesc2.shaders[1].stage = EShaderStage::SHADER_STAGE_FRAGMENT;

		pRenderer->createShader(&mDeferredShader, &shaderDesc2);

		// Create input assembler layout
		FInputAttributeDescription iaAttributes[2];
		iaAttributes[0].binding = 0;
		iaAttributes[0].format = TEXTURE_FORMAT_RG32_FLOAT;
		iaAttributes[0].location = 0;
		iaAttributes[0].offset = 0;

		iaAttributes[1].binding = 0;
		iaAttributes[1].format = TEXTURE_FORMAT_RG32_FLOAT;
		iaAttributes[1].location = 1;
		iaAttributes[1].offset = sizeof(glm::vec2);


		// Create input assembler binding
		FInputBindingDescription iaBinding;
		iaBinding.binding = 0;
		iaBinding.inputRate = EVertexInputRate::VERTEX_INPUT_RATE_VERTEX;
		iaBinding.stride = sizeof(glm::vec2) * 2;

		// Create input assembler layout
		FInputAssemblerLayout iaLayout;
		iaLayout.numInputBindingDescriptions = 1;
		iaLayout.pInputBindingDescriptions = &iaBinding;
		iaLayout.numInputAttributeDescriptions = 2;
		iaLayout.pInputAttributeDescriptions = &iaAttributes[0];

		// Create descriptor bindings.
		FDescriptorLayoutBinding bindings[5];
		bindings[0].set = 0;
		bindings[0].binding = 0;
		bindings[0].descriptorCount = 1;
		bindings[0].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		bindings[0].stageFlags = SHADER_STAGE_FLAG_VERTEX | SHADER_STAGE_FLAG_FRAGMENT;

		bindings[1].set = 1;
		bindings[1].binding = 0;
		bindings[1].descriptorCount = 1;
		bindings[1].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		bindings[1].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		bindings[2].set = 1;
		bindings[2].binding = 1;
		bindings[2].descriptorCount = 1;
		bindings[2].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		bindings[2].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		bindings[3].set = 1;
		bindings[3].binding = 2;
		bindings[3].descriptorCount = 1;
		bindings[3].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLER;
		bindings[3].ppImmutableSamplers = &mSampler;
		bindings[3].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		bindings[4].set = 1;
		bindings[4].binding = 3;
		bindings[4].descriptorCount = 1;
		bindings[4].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLER;
		bindings[4].ppImmutableSamplers = &mSampler;
		bindings[4].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		// Create root signature.
		FRootSignatureCreateDesc rsDesc;
		rsDesc.pShaderProgram = mShader;
		rsDesc.pInputAssemblerLayout = &iaLayout;
		rsDesc.numDescriptorBindings = 5;
		rsDesc.pDescriptorBindings = &bindings[0];

		pRenderer->createRootSignature(&mRootSignature, &rsDesc);

		// Create descriptor bindings.
		FDescriptorLayoutBinding bindings2[7];
		bindings2[0].set = 0;
		bindings2[0].binding = 0;
		bindings2[0].descriptorCount = 1;
		bindings2[0].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		bindings2[0].stageFlags = SHADER_STAGE_FLAG_VERTEX | SHADER_STAGE_FLAG_FRAGMENT;

		bindings2[1].set = 1;
		bindings2[1].binding = 0;
		bindings2[1].descriptorCount = 1;
		bindings2[1].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		bindings2[1].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		bindings2[2].set = 1;
		bindings2[2].binding = 1;
		bindings2[2].descriptorCount = 1;
		bindings2[2].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		bindings2[2].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		bindings2[3].set = 1;
		bindings2[3].binding = 2;
		bindings2[3].descriptorCount = 1;
		bindings2[3].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		bindings2[3].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		bindings2[4].set = 1;
		bindings2[4].binding = 3;
		bindings2[4].descriptorCount = 1;
		bindings2[4].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		bindings2[4].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		bindings2[5].set = 1;
		bindings2[5].binding = 4;
		bindings2[5].descriptorCount = 1;
		bindings2[5].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLER;
		bindings2[5].ppImmutableSamplers = &mSampler;
		bindings2[5].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		bindings2[6].set = 1;
		bindings2[6].binding = 5;
		bindings2[6].descriptorCount = 1;
		bindings2[6].descriptorType = EDescriptorType::DESCRIPTOR_TYPE_SAMPLER;
		bindings2[6].ppImmutableSamplers = &mSampler;
		bindings2[6].stageFlags = SHADER_STAGE_FLAG_FRAGMENT;

		// Create root signature.
		FRootSignatureCreateDesc rsDesc2;
		rsDesc2.pShaderProgram = mDeferredShader;
		rsDesc2.pInputAssemblerLayout = &iaLayout;
		rsDesc2.numDescriptorBindings = 7;
		rsDesc2.pDescriptorBindings = &bindings2[0];

		pRenderer->createRootSignature(&mDeferredRootSignature, &rsDesc2);

		// Create FS quad.
		glm::vec2 FSQuadVertices[] = {
			glm::vec2(-1.0f, -1.0f), glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, -1.0f), glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f),
			glm::vec2(-1.0f, 1.0f), glm::vec2(0.0f, 1.0f),
		};

		FBufferCreateDesc vbDesc;
		vbDesc.usage = BUFFER_USAGE_VERTEX_BUFFER | BUFFER_USAGE_TRANSFER_DST;
		vbDesc.memoryType = MEMORY_TYPE_GPU;
		vbDesc.pData = &FSQuadVertices;
		vbDesc.dataSize = sizeof(glm::vec2) * 8;
		vbDesc.size = vbDesc.dataSize;

		pRenderer->createBuffer(&mFSVertexBuffer, &vbDesc);

		uint32_t FSQuadIndices[] = {
			0, 1, 2, 0, 2, 3
		};

		FBufferCreateDesc ibDesc;
		ibDesc.usage = BUFFER_USAGE_INDEX_BUFFER | BUFFER_USAGE_TRANSFER_DST;
		ibDesc.memoryType = MEMORY_TYPE_GPU;
		ibDesc.pData = &FSQuadIndices;
		ibDesc.dataSize = sizeof(glm::uint32_t) * 6;
		ibDesc.size = ibDesc.dataSize;

		pRenderer->createBuffer(&mFSIndexBuffer, &ibDesc);

		const uint32_t ssaoNoiseAmount = 4;

		// SSAO
		std::default_random_engine rndEngine((unsigned)time(nullptr));
		std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);

		std::vector<glm::vec4> ssaoNoise(ssaoNoiseAmount * ssaoNoiseAmount);
		for (uint32_t i = 0; i < static_cast<uint32_t>(ssaoNoise.size()); i++)
		{
			ssaoNoise[i] = glm::vec4(rndDist(rndEngine) * 2.0f - 1.0f, rndDist(rndEngine) * 2.0f - 1.0f, 0.0f, 0.0f);
		}

		// Sample kernel
		for (uint32_t i = 0; i < 32; ++i)
		{
			glm::vec3 sample(rndDist(rndEngine) * 2.0 - 1.0, rndDist(rndEngine) * 2.0 - 1.0, rndDist(rndEngine));
			sample = glm::normalize(sample);
			sample *= rndDist(rndEngine);
			float scale = float(i) / float(32);
			scale = Lerp(0.1f, 1.0f, scale * scale);
			mConstants.cKernels[i] = glm::vec4(sample * scale, 0.0f);
		}

		FImageCreateDesc texDesc = {};
		texDesc.type = IMAGE_TYPE_2D;
		texDesc.memoryType = MEMORY_TYPE_GPU;
		texDesc.arrayLayers = 1;
		texDesc.baseArrayLayer = 0;
		texDesc.baseMipLevel = 0;
		texDesc.depth = 1;
		texDesc.format = TEXTURE_FORMAT_RGBA32_FLOAT;
		texDesc.width = ssaoNoiseAmount;
		texDesc.height = ssaoNoiseAmount;
		texDesc.mipLevels = 1;
		texDesc.pData = ssaoNoise.data();
		texDesc.dataSize = ssaoNoise.size() * sizeof(glm::vec4);
		texDesc.sampleCount = 1;
		texDesc.sampleQuality = 0;
		texDesc.tiling = IMAGE_TILING_OPTIMAL;
		texDesc.usage = IMAGE_USAGE_SAMPLED | IMAGE_USAGE_TRANSFER_DST;

		pRenderer->createImage(&mRandomTexture, &texDesc);
		

		mRaserizerState.cullMode = ECullMode::CULL_MODE_NONE;
		mRaserizerState.depthClampEnable = false;
		mRaserizerState.rasterizerDiscardEnable = false;
		mRaserizerState.polygonMode = POLYGON_MODE_FILL;
		mRaserizerState.lineWidth = 1.0f;
		mRaserizerState.frontFace = EFrontFaceWinding::FRONT_FACE_WINDING_CLOCKWISE;
		mRaserizerState.depthBiasEnable = false;
		mRaserizerState.depthBiasConstantFactor = 0.0f; // Optional
		mRaserizerState.depthBiasClamp = 0.0f; // Optional
		mRaserizerState.depthBiasSlopeFactor = 0.0f; // Optional
	}

	void RGMSSAO::Destroy(IRenderInterface* pRenderer)
	{
		pRenderer->destroyShader(mShader);
		pRenderer->destroyShader(mDeferredShader);
		pRenderer->destroySampler(mSampler);
		pRenderer->destroyRootSignature(mRootSignature);
		pRenderer->destroyRootSignature(mDeferredRootSignature);
		pRenderer->destroyBuffer(mFSIndexBuffer);
		pRenderer->destroyBuffer(mFSVertexBuffer);
		pRenderer->destroyImage(mRandomTexture);

	}

	void RGMSSAO::Load(IRenderInterface* pRenderer, ISwapChain* pSwapChain)
	{

		// Create graphics pipeline.
		FGraphicsPipelineCreateDesc pipelineDesc = {};
		pipelineDesc.pRootSignature = mRootSignature;
		pipelineDesc.pShader = mShader;
		pipelineDesc.pBlendState = nullptr;
		pipelineDesc.pDepthStencilState = nullptr;
		pipelineDesc.pRasterizerState = &mRaserizerState;
		pipelineDesc.depthStencilFormat = TEXTURE_FORMAT_UNKNOWN;
		pipelineDesc.numRenderTargets = 1;
		pipelineDesc.renderTargetFormats[0] = TEXTURE_FORMAT_R32_FLOAT;
		;
		pipelineDesc.primitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE;
		pipelineDesc.sampleCount = pSwapChain->getRenderTarget(0)->GetDesc()->sampleCount;
		pipelineDesc.sampleQuality = pSwapChain->getRenderTarget(0)->GetDesc()->sampleQuality;

		pRenderer->createPipeline(&mPipeline, &pipelineDesc);

		pipelineDesc.pRootSignature = mDeferredRootSignature;
		pipelineDesc.pShader = mDeferredShader;

		pRenderer->createPipeline(&mDeferredPipeline, &pipelineDesc);
	}

	void RGMSSAO::Unload(IRenderInterface* pRenderer)
	{
		pRenderer->destroyPipeline(mPipeline);
		pRenderer->destroyPipeline(mDeferredPipeline);
	}

	FRGMSSAOTargets RGMSSAO::RGDrawSSAO_Depth(RRenderGraphBuilder* pBuilder, FVirtualResourceRef* depth, RCamera* camera, FSSAOVars& vars)
	{
		FRGMSSAOTargets output;

		auto create = pBuilder->add_pass("create_ssao_target", [&](RRenderPassBuilder& builder)
		{
			FVirtualTextureDesc depthInfo = depth->mResource->mDesc.GetTextureDesc();

			// create resource.
			FVirtualResourceDesc desc;
			desc.mType = EVirtualResourceType::VIRTUAL_RESOURCE_TYPE_TEXTURE;
			desc.mTextureDesc.width = depthInfo.width;
			desc.mTextureDesc.height = depthInfo.height;
			desc.mTextureDesc.depth = 1;
			desc.mTextureDesc.format = ETextureFormat::TEXTURE_FORMAT_R32_FLOAT;
			desc.mTextureDesc.sampleCount = 1;
			desc.mTextureDesc.sampleQuality = 1;

			// import resource.
			output.pSSAO = builder.create_resource("ssao", desc);
		});

		// Add the pass and setup its inputs and outputs.
		auto pass = pBuilder->add_pass("draw_ssao_forward", [&](RRenderPassBuilder& builder)
		{
			output.pSSAO = builder.write_resource("ssao", output.pSSAO, RENDER_GRAPH_RESOURCE_ACCESS_GPU, RENDER_GRAPH_RESOURCE_WRITE_STATE_COLOR_ATTACHMENT);
			output.pDepth = builder.read_resource("depth", depth, RENDER_GRAPH_RESOURCE_ACCESS_GPU, RENDER_GRAPH_RESOURCE_READ_STATE_SAMPLED);

		});

		// Set execute function.
		pass->setExecuteFunc([=](IRenderInterface* renderer, RRenderGraph& graph, RCompiledPass& pass, ICommandList* cmd)
		{
			FLoadStoreOps loadStoreOps = {};
			loadStoreOps.colorLoadOps[0] = LOAD_OP_CLEAR;
			loadStoreOps.colorStoreOps[0] = STORE_OP_STORE;
			loadStoreOps.colorClearValues[0][0] = 0.0f;
			loadStoreOps.colorClearValues[0][1] = 0.0f;
			loadStoreOps.colorClearValues[0][2] = 0.0f;
			loadStoreOps.colorClearValues[0][3] = 0.0f;
			loadStoreOps.depthLoadOp = LOAD_OP_LOAD;
			loadStoreOps.depthStoreOp = STORE_OP_STORE;
			loadStoreOps.depthClearValue = 1.0f;

			IRenderTarget* ssao = pass.getRenderTargetResource("ssao");
			IRenderTarget* depth = pass.getRenderTargetResource("depth");

			FImageBarrier texBarriers[1];
			texBarriers[0].requiredState = RESOURCE_STATE_SHADER_RESOURCE;
			texBarriers[0].image = mRandomTexture;

			renderer->cmdResourceBarriers(cmd, 0, nullptr, 1, &texBarriers[0], true);

			// Begin rendering pass.
			renderer->cmdBeginRenderPass(cmd, 1, &ssao, nullptr, &loadStoreOps);

			// Bind graphics pipeline.
			renderer->cmdBindPipeline(cmd, mPipeline);
			renderer->cmdSetViewport(cmd, FRect({ ssao->GetDesc()->width,  ssao->GetDesc()->height }, { 0, 0 }));
			renderer->cmdSetScissorRects(cmd, FRect({ ssao->GetDesc()->width, ssao->GetDesc()->height }, { 0, 0 }));
			
			glm::mat4 invMvp = glm::inverse(camera->getProjectionMatrix());

			glm::vec4 farTopLeftVert = invMvp * glm::vec4(-1, 1, 1, 1);
			glm::vec4 farTopRightVert = invMvp * glm::vec4(1, 1, 1, 1);
			glm::vec4 farBottomLeftVert = invMvp * glm::vec4(-1, -1, 1, 1);
			glm::vec4 farBottomRightVert = invMvp * glm::vec4(1, -1, 1, 1);

			farTopLeftVert /= farTopLeftVert.w;
			farTopRightVert /= farTopRightVert.w;
			farBottomLeftVert /= farBottomLeftVert.w;
			farBottomRightVert /= farBottomRightVert.w;

			farTopLeftVert.w = 1.f;
			farTopRightVert.w = 1.f;
			farBottomLeftVert.w = 1.f;
			farBottomRightVert.w = 1.f;

			// Create dynamic uniform allocation.
			FDynamicUniformAllocation cameraUniformAlloc;

			mConstants.cProjection = camera->getProjectionMatrix();
			mConstants.cInvProjection = invMvp;
			mConstants.cFrustumCorners[0] = farBottomLeftVert;
			mConstants.cFrustumCorners[1] = farBottomRightVert;
			mConstants.cFrustumCorners[2] = farTopRightVert;
			mConstants.cFrustumCorners[3] = farTopLeftVert;

			mConstants.cScreenSize = glm::vec2(ssao->GetDesc()->width, ssao->GetDesc()->height);
			mConstants.cNearZ = camera->getNearZ();
			mConstants.cFarZ = camera->getFarZ();
			mConstants.cRandomSize = vars.cRandomSize;
			mConstants.cSampleRadius = vars.cSampleRadius;
			mConstants.cIntensity = vars.cIntensity;
			mConstants.cScale = vars.cScale;
			mConstants.cBias = vars.cBias;

			srand(0);
			for (int i = 0; i < 32; ++i) 
			{
				mConstants.cKernels[i] = glm::vec4(
					RandomFloat(-1.0f, 1.0f),
					RandomFloat(-1.0f, 1.0f),
					RandomFloat(0.0f, 1.0f),
					0.0f);

				mConstants.cKernels[i] = glm::normalize(mConstants.cKernels[i]);
				mConstants.cKernels[i] = mConstants.cKernels[i] *= RandomFloat(0.0f, 1.0f);

				float scale = float(i) / float(32);
				scale = Lerp(0.1f, 1.0f, scale * scale);
				mConstants.cKernels[i] *= scale;
			}

			renderer->cmdAllocateDynamicUniformBuffer(cmd, &cameraUniformAlloc, sizeof(FSSAOConstants), &mConstants);


			// Bind index/vertex buffer
			renderer->cmdBindIndexBuffer(cmd, mFSIndexBuffer);
			renderer->cmdBindVertexBuffer(cmd, 1, &mFSVertexBuffer);

			IImage* image = depth->GetImage();

			FDescriptorBinding bindings[3];
			bindings[0].set = 0;
			bindings[0].binding = 0;
			bindings[0].numDescriptors = 1;
			bindings[0].type = DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			bindings[0].ppBuffers = &cameraUniformAlloc.buffer;
			bindings[0].range = cameraUniformAlloc.range;
			bindings[0].offset = cameraUniformAlloc.offset;

			bindings[1].set = 1;
			bindings[1].binding = 0;
			bindings[1].numDescriptors = 1;
			bindings[1].type = DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			bindings[1].ppImages = &image;
			bindings[1].range = 0; // range 0 means full range.
			bindings[1].offset = 0;

			bindings[2].set = 1;
			bindings[2].binding = 1;
			bindings[2].numDescriptors = 1;
			bindings[2].type = DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			bindings[2].ppImages = &mRandomTexture;
			bindings[2].range = 0; // range 0 means full range.
			bindings[2].offset = 0;

			// Bind resources.
			renderer->cmdBindDescriptors(cmd, 3, &bindings[0]);

			// Draw mesh.
			renderer->cmdDrawIndexed(cmd, 6, 0, 0);

			// Finish rendering pass.
			renderer->cmdEndRenderPass(cmd);
		});

		return output;
	}

	FRGMDeferredSSAOTargets RGMSSAO::RGDrawSSAO_Deferred(RRenderGraphBuilder* pBuilder, FVirtualResourceRef* depth, FVirtualResourceRef* normal, FVirtualResourceRef* position, RCamera* camera, FSSAOVars& vars)
	{
		FRGMDeferredSSAOTargets output;

		auto create = pBuilder->add_pass("create_ssao_target", [&](RRenderPassBuilder& builder)
		{
			FVirtualTextureDesc depthInfo = depth->mResource->mDesc.GetTextureDesc();

			// create resource.
			FVirtualResourceDesc desc;
			desc.mType = EVirtualResourceType::VIRTUAL_RESOURCE_TYPE_TEXTURE;
			desc.mTextureDesc.width = depthInfo.width;
			desc.mTextureDesc.height = depthInfo.height;
			desc.mTextureDesc.depth = 1;
			desc.mTextureDesc.format = ETextureFormat::TEXTURE_FORMAT_R32_FLOAT;
			desc.mTextureDesc.sampleCount = 1;
			desc.mTextureDesc.sampleQuality = 1;

			// import resource.
			output.pSSAO = builder.create_resource("ssao", desc);
		});

		// Add the pass and setup its inputs and outputs.
		auto pass = pBuilder->add_pass("draw_ssao_forward", [&](RRenderPassBuilder& builder)
		{
			output.pSSAO = builder.write_resource("ssao", output.pSSAO, RENDER_GRAPH_RESOURCE_ACCESS_GPU, RENDER_GRAPH_RESOURCE_WRITE_STATE_COLOR_ATTACHMENT);
			output.pDepth = builder.read_resource("depth", depth, RENDER_GRAPH_RESOURCE_ACCESS_GPU, RENDER_GRAPH_RESOURCE_READ_STATE_SAMPLED);
			output.pPosition = builder.read_resource("position", position, RENDER_GRAPH_RESOURCE_ACCESS_GPU, RENDER_GRAPH_RESOURCE_READ_STATE_SAMPLED);
			output.pNormal = builder.read_resource("normal", normal, RENDER_GRAPH_RESOURCE_ACCESS_GPU, RENDER_GRAPH_RESOURCE_READ_STATE_SAMPLED);
		});

		// Set execute function.
		pass->setExecuteFunc([=](IRenderInterface* renderer, RRenderGraph& graph, RCompiledPass& pass, ICommandList* cmd)
		{
			FLoadStoreOps loadStoreOps = {};
			loadStoreOps.colorLoadOps[0] = LOAD_OP_CLEAR;
			loadStoreOps.colorStoreOps[0] = STORE_OP_STORE;
			loadStoreOps.colorClearValues[0][0] = 1.0f;
			loadStoreOps.colorClearValues[0][1] = 1.0f;
			loadStoreOps.colorClearValues[0][2] = 1.0f;
			loadStoreOps.colorClearValues[0][3] = 1.0f;
			loadStoreOps.depthLoadOp = LOAD_OP_LOAD;
			loadStoreOps.depthStoreOp = STORE_OP_STORE;
			loadStoreOps.depthClearValue = 1.0f;

			IRenderTarget* ssao = pass.getRenderTargetResource("ssao");
			IRenderTarget* depth = pass.getRenderTargetResource("depth");
			IRenderTarget* position = pass.getRenderTargetResource("position");
			IRenderTarget* normal = pass.getRenderTargetResource("normal");

			FImageBarrier texBarriers[1];
			texBarriers[0].requiredState = RESOURCE_STATE_SHADER_RESOURCE;
			texBarriers[0].image = mRandomTexture;

			renderer->cmdResourceBarriers(cmd, 0, nullptr, 1, &texBarriers[0], true);

			// Begin rendering pass.
			renderer->cmdBeginRenderPass(cmd, 1, &ssao, nullptr, &loadStoreOps);

			// Bind graphics pipeline.
			renderer->cmdBindPipeline(cmd, mDeferredPipeline);
			renderer->cmdSetViewport(cmd, FRect({ ssao->GetDesc()->width,  ssao->GetDesc()->height }, { 0, 0 }));
			renderer->cmdSetScissorRects(cmd, FRect({ ssao->GetDesc()->width, ssao->GetDesc()->height }, { 0, 0 }));

			glm::mat4 invMvp = glm::inverse(camera->getProjectionMatrix());

			glm::vec4 farTopLeftVert = invMvp * glm::vec4(-1, 1, 1, 1);
			glm::vec4 farTopRightVert = invMvp * glm::vec4(1, 1, 1, 1);
			glm::vec4 farBottomLeftVert = invMvp * glm::vec4(-1, -1, 1, 1);
			glm::vec4 farBottomRightVert = invMvp * glm::vec4(1, -1, 1, 1);

			farTopLeftVert /= farTopLeftVert.w;
			farTopRightVert /= farTopRightVert.w;
			farBottomLeftVert /= farBottomLeftVert.w;
			farBottomRightVert /= farBottomRightVert.w;

			farTopLeftVert.w = 1.f;
			farTopRightVert.w = 1.f;
			farBottomLeftVert.w = 1.f;
			farBottomRightVert.w = 1.f;

			// Create dynamic uniform allocation.
			FDynamicUniformAllocation cameraUniformAlloc;


			mConstants.cProjection = camera->getProjectionMatrix();
			mConstants.cInvProjection = invMvp;
			mConstants.cFrustumCorners[0] = farBottomLeftVert;
			mConstants.cFrustumCorners[1] = farBottomRightVert;
			mConstants.cFrustumCorners[2] = farTopRightVert;
			mConstants.cFrustumCorners[3] = farTopLeftVert;

			mConstants.cScreenSize = glm::vec2(ssao->GetDesc()->width, ssao->GetDesc()->height);
			mConstants.cNearZ = camera->getNearZ();
			mConstants.cFarZ = camera->getFarZ();
			mConstants.cRandomSize = vars.cRandomSize;
			mConstants.cSampleRadius = vars.cSampleRadius;
			mConstants.cIntensity = vars.cIntensity;
			mConstants.cScale = vars.cScale;
			mConstants.cBias = vars.cBias;

			renderer->cmdAllocateDynamicUniformBuffer(cmd, &cameraUniformAlloc, sizeof(FSSAOConstants), &mConstants);


			// Bind index/vertex buffer
			renderer->cmdBindIndexBuffer(cmd, mFSIndexBuffer);
			renderer->cmdBindVertexBuffer(cmd, 1, &mFSVertexBuffer);

			IImage* image = depth->GetImage();

			FDescriptorBinding bindings[5];
			bindings[0].set = 0;
			bindings[0].binding = 0;
			bindings[0].numDescriptors = 1;
			bindings[0].type = DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			bindings[0].ppBuffers = &cameraUniformAlloc.buffer;
			bindings[0].range = cameraUniformAlloc.range;
			bindings[0].offset = cameraUniformAlloc.offset;

			bindings[1].set = 1;
			bindings[1].binding = 0;
			bindings[1].numDescriptors = 1;
			bindings[1].type = DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			bindings[1].ppImages = &image;
			bindings[1].range = 0; // range 0 means full range.
			bindings[1].offset = 0;

			bindings[2].set = 1;
			bindings[2].binding = 1;
			bindings[2].numDescriptors = 1;
			bindings[2].type = DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			bindings[2].ppImages = &mRandomTexture;
			bindings[2].range = 0; // range 0 means full range.
			bindings[2].offset = 0;

			IImage* positionTexture = position->GetImage();
			
			bindings[3].set = 1;
			bindings[3].binding = 2;
			bindings[3].numDescriptors = 1;
			bindings[3].type = DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			bindings[3].ppImages = &positionTexture;
			bindings[3].range = 0; // range 0 means full range.
			bindings[3].offset = 0;

			IImage* normalTexture = normal->GetImage();
	
			bindings[4].set = 1;
			bindings[4].binding = 3;
			bindings[4].numDescriptors = 1;
			bindings[4].type = DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			bindings[4].ppImages = &normalTexture;
			bindings[4].range = 0; // range 0 means full range.
			bindings[4].offset = 0;

			// Bind resources.
			renderer->cmdBindDescriptors(cmd, 5, &bindings[0]);

			// Draw mesh.
			renderer->cmdDrawIndexed(cmd, 6, 0, 0);

			// Finish rendering pass.
			renderer->cmdEndRenderPass(cmd);
		});

		return output;
	}
}
